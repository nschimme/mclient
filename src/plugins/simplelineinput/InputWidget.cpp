#include "SimpleLineInput.h"
#include "InputWidget.h"

#include <QDebug>

InputWidget::InputWidget(QString s, SimpleLineInput* sli, QWidget* parent) 
    : QLineEdit(parent) {
    _session = s;
    _sli = sli;

    setMaxLength(255);

    // Connect Signals/Slots
    connect(this, SIGNAL(returnPressed()), SLOT(gotInput()));
    connect(this, SIGNAL(sendUserInput(const QString&)),
	    _sli, SLOT(sendUserInput(const QString&)));

    // Debugging Information
    qDebug() << "* SimpleLineInput thread:" << _sli->thread();
    qDebug() << "* InputWidget thread:" << this->thread();
}


InputWidget::~InputWidget() {
}

void InputWidget::gotInput() {
    selectAll();
    emit sendUserInput(text());
}
