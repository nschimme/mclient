#include "PasswordWidget.h"

#include <QDebug>

PasswordWidget::PasswordWidget(QWidget* parent) : QLineEdit(parent) {
    setMaxLength(255);
    setEchoMode(QLineEdit::Password);

    connect(this, SIGNAL(returnPressed()), SLOT(gotInput()));
}


PasswordWidget::~PasswordWidget() {
}


void PasswordWidget::gotInput() {
    selectAll();
    emit sendUserInput(text(), false);
    clear();
}
