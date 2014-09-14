#include "InputPasswordWidget.h"

#include <QDebug>

InputPasswordWidget::InputPasswordWidget(QWidget* parent) : QLineEdit(parent) {
    setMaxLength(255);
    setEchoMode(QLineEdit::Password);

    connect(this, SIGNAL(returnPressed()), SLOT(gotInput()));
}


InputPasswordWidget::~InputPasswordWidget() {
}


void InputPasswordWidget::gotInput() {
    selectAll();
    emit sendUserInput(text(), false);
    clear();
}
