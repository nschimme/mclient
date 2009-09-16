#include "SimpleLineInput.h"
#include "InputWidget.h"

#include <QDebug>

InputWidget::InputWidget(QWidget* parent) : QLineEdit(parent) {
    setMaxLength(255);
    _echoMode = true;

    connect(this, SIGNAL(returnPressed()), SLOT(gotInput()));
}


InputWidget::~InputWidget() {
}

void InputWidget::gotInput() {
    selectAll();
    emit sendUserInput(text(), _echoMode);
}

void InputWidget::toggleEchoMode(bool b) {
  _echoMode = b;
  clear();
  if (_echoMode) {
    setEchoMode(QLineEdit::Normal);

  }
  else {
    setEchoMode(QLineEdit::Password);

  }
}
