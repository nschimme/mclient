#include "InputWidget.h"

#include <QDebug>
#include <QKeyEvent>

#include <QTextCharFormat>
#include <QFont>
#include <QFontMetrics>
#include <QTextCursor>

// Smart Splitter
#include <QSizePolicy>

// Word History
#include <QStringList>

InputWidget::InputWidget(QWidget* parent) 
    : QPlainTextEdit(parent) {

  // Size Policy
  QFontMetrics fm(currentCharFormat().font());
  int textHeight = fm.height();
  int documentMargin = document()->documentMargin()*2;
  int frameWidth = QFrame::frameWidth()*2;
  
  QSizePolicy policy = QSizePolicy(QSizePolicy::Expanding,
				   QSizePolicy::Maximum);
  setSizePolicy(policy);
  QWidget::setMinimumSize(0, textHeight+documentMargin+frameWidth);
  QWidget::setBaseSize(0, documentMargin+frameWidth +2); // +2 just because
  QWidget::setSizeIncrement(0, textHeight);

  // Line Wrapping
  setLineWrapMode(QPlainTextEdit::NoWrap);
  
  // Local Echo?
  _echoMode = true;
    
  // Word History
  _wordHistory << "dork" << "double" << "doobie";
  _lineIterator = new QMutableStringListIterator(_lineHistory);
  _tabIterator = new QMutableStringListIterator(_wordHistory);
  _newInput = true;
  
}


QSize InputWidget::sizeHint() const {
  return minimumSize();
}


InputWidget::~InputWidget() {
  delete _lineIterator;
  delete _tabIterator;
}


void InputWidget::keyPressEvent(QKeyEvent *event) {
  if (event->key() !=  Qt::Key_Tab) _tabState = NORMAL;

  // Check for key bindings
  switch (event->key()) {
    /** Enter could mean to submit the current text or add a newline */
  case Qt::Key_Return:
  case Qt::Key_Enter:
    
    switch (event->modifiers()) {
    case Qt::NoModifier:    /** Submit text */
      gotInput();
      detectedLineChange();
      break;
      
    case Qt::ShiftModifier: /** Add newline (i.e. ignore) */

    default:                /** Otherwise ignore  */
      QPlainTextEdit::keyPressEvent(event);
      detectedLineChange();
      
    };
    break;

    /** Key bindings for word history, tab completion, etc) */
  case Qt::Key_Up:
  case Qt::Key_Down:
  case Qt::Key_Tab:

    switch (event->modifiers()) {
    case Qt::KeypadModifier:
      keypadMovement(event->key());
      break;
    case Qt::NoModifier:
      wordHistory(event->key());
      break;
    default:
      QPlainTextEdit::keyPressEvent(event);
    };
    break;

  case Qt::Key_Left:
  case Qt::Key_Right:
  case Qt::Key_PageUp:
  case Qt::Key_PageDown:
  case Qt::Key_Clear:  // Numpad 5
  case Qt::Key_Home:
  case Qt::Key_End:
    // TODO, Implement these following keys
  case Qt::Key_Delete:
  case Qt::Key_Plus:
  case Qt::Key_Minus:
  case Qt::Key_Slash:
  case Qt::Key_Asterisk:
  case Qt::Key_Insert:
    switch (event->modifiers()) {
    case Qt::KeypadModifier:
      keypadMovement(event->key());
      break; 
    };

    /** All other keys */
  default:
    QPlainTextEdit::keyPressEvent(event);
  };
}


void InputWidget::keypadMovement(int key) {
  switch (key) {
  case Qt::Key_Up:
    emit sendUserInput("north", _echoMode);
    break;
  case Qt::Key_Down:
    emit sendUserInput("south", _echoMode);
    break;
  case Qt::Key_Left:
    emit sendUserInput("west", _echoMode);
    break;
  case Qt::Key_Right:
    emit sendUserInput("east", _echoMode);
    break;
  case Qt::Key_PageUp:
    emit sendUserInput("up", _echoMode);
    break;
  case Qt::Key_PageDown:
    emit sendUserInput("down", _echoMode);
    break;
  case Qt::Key_Clear: // Numpad 5
    emit sendUserInput("exits", _echoMode);
    break;
  case Qt::Key_Home:
    emit sendUserInput("open exit", _echoMode);
    break;
  case Qt::Key_End:
    emit sendUserInput("close exit", _echoMode);
    break;
  case Qt::Key_Insert:
    emit sendUserInput("flee", _echoMode);
    break;
  case Qt::Key_Delete:
  case Qt::Key_Plus:
  case Qt::Key_Minus:
  case Qt::Key_Slash:
  case Qt::Key_Asterisk:
  default:
    qDebug() << "! Unknown keypad movement" << key;
  };
}


void InputWidget::wordHistory(int key) {
  QTextCursor cursor = textCursor();
  switch (key) {
  case Qt::Key_Up:
    if (!cursor.movePosition(QTextCursor::Up)) {
      // At the top of the document
      backwardHistory();

    }
    break;
  case Qt::Key_Down:
    if (!cursor.movePosition(QTextCursor::Down)) {
      // At the end of the document
      forwardHistory();

    }
    break;
  case Qt::Key_Tab:
    tabWord();
    break;
  };
}


void InputWidget::detectedLineChange() {
  /*
  qDebug() << "InputWidget resizing!";
  QSize size = document()->documentLayout()->documentSize().toSize();
  int height = baseSize().height() + size.height() * sizeIncrement().height();

  qDebug() << QWidget::size() << height;  
  resize(QWidget::width(), height);
  emit resizeSplitter(this);
  */
}


void InputWidget::gotInput() {
  selectAll();
  emit sendUserInput(toPlainText(), _echoMode);
  if (_echoMode) {
    addLineHistory(toPlainText());
    addTabHistory(toPlainText());
    _lineIterator->toBack();
  }

}


void InputWidget::toggleEchoMode(bool b) {
  _echoMode = b;
  clear();
  if (_echoMode) {
    //setEchoMode(QLineEdit::Normal);

  }
  else {
    //setEchoMode(QLineEdit::Password);

  }
}


void InputWidget::addLineHistory(const QString &string) {
  if (!string.isEmpty()) {
    qDebug() << "* adding line history:" << string;
    _lineHistory << string;
  }

  if (_lineHistory.size() > 100) _lineHistory.removeFirst();
}


void InputWidget::addTabHistory(const QString &string) {
  QStringList list = string.split(QRegExp("\\W+"), QString::SkipEmptyParts);
  addTabHistory(list);
}


void InputWidget::addTabHistory(const QStringList &list) {
  foreach (QString word, list)
    if (word.length() > 3) {
      qDebug() << "* adding word history:" << word;
      _wordHistory << word;
    }

  while (_wordHistory.size() > 200) _wordHistory.removeFirst();
}


void InputWidget::forwardHistory() {
  if (!_lineIterator->hasNext()) {
    qDebug() << "* no newer word history to go to";
    clear();
    return ;

  }
  
  if (_newInput) {
    selectAll();
    addLineHistory(toPlainText());
    
  }

  selectAll();
  QString next = _lineIterator->next();
  // Ensure we always get "new" input
  if (next == toPlainText() &&
      _lineIterator->hasNext()) next = _lineIterator->next();

  insertPlainText(next);
  _newInput = false;
  
}


void InputWidget::backwardHistory() {
  if (!_lineIterator->hasPrevious()) {
    qDebug() << "* no older word history to go to";
    return ;

  }
  
  if (_newInput) {
    selectAll();
    addLineHistory(toPlainText());
    
  }

  selectAll();
  QString previous = _lineIterator->previous();
  // Ensure we always get "new" input
  if (previous == toPlainText() &&
      _lineIterator->hasPrevious()) previous = _lineIterator->previous();

  insertPlainText(previous);
  _newInput = false;

}


void InputWidget::showCommandHistory() {
  QString message("#history:\r\n");
  int i = 1;
  foreach (QString s, _lineHistory)
    message.append(QString::number(i++) + " " + s + "\r\n");

  qDebug() << "* showing history" << message;

  emit displayMessage(message);

}


void InputWidget::tabWord() {
  QTextCursor current = textCursor();
  current.select(QTextCursor::WordUnderCursor);
  switch (_tabState) {
  case NORMAL:
    qDebug() << "* in NORMAL state";
    _tabFragment = current.selectedText();
    _tabIterator->toBack();
    _tabState = TABBING;
  case TABBING:
    qDebug() << "* in TABBING state";
    if (!_tabIterator->hasPrevious() && !_wordHistory.isEmpty()) {
      qDebug() << "* Resetting tab iterator";
      _tabIterator->toBack();
    }
    while (_tabIterator->hasPrevious()) {
      if (_tabIterator->peekPrevious().startsWith(_tabFragment)) {
	current.insertText(_tabIterator->previous());
	if (current.movePosition(QTextCursor::StartOfWord,
				 QTextCursor::KeepAnchor)) {
	  current.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor,
			       _tabFragment.size());
	  setTextCursor(current);
	  qDebug() << "*it worked";
	}
	return ;
      }
      else _tabIterator->previous();
    }
    break;
  };
}
