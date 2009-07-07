#include "MultiLineInput.h"
#include "InputWidget.h"

#include <QDebug>
#include <QKeyEvent>

#include <QTextCharFormat>
#include <QFont>
#include <QFontMetrics>

#include <QSizePolicy>

InputWidget::InputWidget(QString s, MultiLineInput* mli, QWidget* parent) 
    : QPlainTextEdit(parent) {
    _session = s;
    _mli = mli;
    
    QFontMetrics fm(currentCharFormat().font());
    _textPixelHeight = fm.height();

    QSizePolicy policy = QSizePolicy(QSizePolicy::Expanding,
				     QSizePolicy::Minimum);
    setSizePolicy(policy);
    QWidget::setMinimumSize(0, _textPixelHeight + 13);

    // Connect Signals/Slots
    connect(this, SIGNAL(sendUserInput(const QString&, bool)),
	    _mli, SLOT(sendUserInput(const QString&, bool)));
    connect(_mli, SIGNAL(setEchoMode(bool)),
	    this, SLOT(toggleEchoMode(bool)));

    // Debugging Information
    qDebug() << "* MultiLineInput thread:" << _mli->thread();
    qDebug() << "* InputWidget thread:" << this->thread();
}


QSize InputWidget::sizeHint() const {
  return QSize(0, _textPixelHeight + 13);
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
      break;
      
    case Qt::ShiftModifier: /** Add newline (i.e. ignore) */

    default:                /** Otherwise ignore  */
      // detectedLineChange();
      QPlainTextEdit::keyPressEvent(event);
      break;
      
    };
    break;
    
    /** All other keys */
  default:
    QPlainTextEdit::keyPressEvent(event);
    break;
  };
}


void InputWidget::detectedLineChange() {
  QSize size = document()->documentLayout()->documentSize().toSize();
  int height = size.height() * _textPixelHeight + 10;

  qDebug() << QWidget::minimumSize() << QWidget::maximumSize()
	   << QWidget::size() << height;

  resize(QWidget::width(), height);

}


void InputWidget::gotInput() {
  selectAll();
  emit sendUserInput(toPlainText(), _echoMode);
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
