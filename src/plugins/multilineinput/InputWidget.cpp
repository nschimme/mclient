#include "MultiLineInput.h"
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

InputWidget::InputWidget(QString s, MultiLineInput* mli, QWidget* parent) 
    : QPlainTextEdit(parent) {
    _session = s;
    _mli = mli;

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

    // Debugging Information
    qDebug() << "* MultiLineInput thread:" << _mli->thread();
    qDebug() << "* InputWidget thread:" << this->thread();

    // Local Echo?
    _echoMode = true;

    // Word History
    _wordHistory << "lol" << "dork";
    _iterator = new QMutableStringListIterator(_wordHistory);
    _newInput = true;

}


QSize InputWidget::sizeHint() const {
  return minimumSize();
}


InputWidget::~InputWidget() {
}


void InputWidget::keyPressEvent(QKeyEvent *event) {
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
    case Qt::NoModifier:
      wordHistory(event->key());
      break;
    default:
      QPlainTextEdit::keyPressEvent(event);
    };
    break;
    
    /** All other keys */
  default:
    QPlainTextEdit::keyPressEvent(event);
  };
}


void InputWidget::wordHistory(int key) {
  QTextCursor cursor = textCursor();
  switch (key) {
  case Qt::Key_Up:
    if (!cursor.movePosition(QTextCursor::Up)) {
      // At the top of the document
      forwardHistory();

    }
    break;
  case Qt::Key_Down:
    if (!cursor.movePosition(QTextCursor::Down)) {
      // At the end of the document
      backwardHistory();

    }
    break;
  case Qt::Key_Tab:
    break;
  };
}


void InputWidget::detectedLineChange() {
  qDebug() << "InputWidget resizing!";
  QSize size = document()->documentLayout()->documentSize().toSize();
  int height = baseSize().height() + size.height() * sizeIncrement().height();

  qDebug() << QWidget::size() << height;
  
  resize(QWidget::width(), height);
  emit resizeSplitter(this);

}


void InputWidget::gotInput() {
  selectAll();
  emit sendUserInput(toPlainText(), _echoMode);
  addHistory(toPlainText());
  _iterator->toBack();

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


void InputWidget::addHistory(const QString string) {
  if (!string.isEmpty()) {
    qDebug() << "* adding history:" << string;
    _wordHistory << string;
  }

  if (_wordHistory.size() > 100) _wordHistory.removeFirst();

}


void InputWidget::forwardHistory() {
  if (!_iterator->hasNext()) {
    qDebug() << "* no newer word history to go to";
    return ;

  }
  
  if (_newInput) {
    selectAll();
    addHistory(toPlainText());
    
  }

  selectAll();
  insertPlainText(_iterator->next());
  _newInput = false;
  
}


void InputWidget::backwardHistory() {
  if (!_iterator->hasPrevious()) {
    qDebug() << "* no older word history to go to";
    return ;

  }
  
  if (_newInput) {
    selectAll();
    addHistory(toPlainText());
    
  }

  selectAll();
  insertPlainText(_iterator->previous());
  _newInput = false;

}


void InputWidget::showCommandHistory() {
  QString message("#history:\r\n");
  int i = 1;
  foreach (QString s, _wordHistory)
    message.append(QString::number(i++) + " " + s + "\r\n");

  qDebug() << "* showing history" << message;

  emit displayMessage(message);

}
