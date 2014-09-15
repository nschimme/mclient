#include "InputWidget.h"

#include "InputMultiWidget.h"
#include <QDebug>

InputWidget::InputWidget(QWidget* parent)
    : QStackedWidget(parent), _localEcho(true) {

    // Multiline Input Widget
    _inputWidget = new InputMultiWidget(this);
    addWidget(_inputWidget);
    /*
     *     void sendUserInput(const QString&);
    void resizeSplitter(QWidget*);
    void displayMessage(const QString &);
    */
    connect(_inputWidget, SIGNAL(sendUserInput(QString)),
            SLOT(gotMultiLineInput(QString)));
    connect(_inputWidget, SIGNAL(displayMessage(QString)),
            SLOT(relayMessage(QString)));

    // Password Widget
    _passwordWidget = new QLineEdit(this);
    _passwordWidget->setMaxLength(255);
    _passwordWidget->setEchoMode(QLineEdit::Password);
    addWidget(_passwordWidget);
    connect(_passwordWidget, SIGNAL(returnPressed()),
            SLOT(gotPasswordInput()));

    // Grab focus
    setFocusProxy(_inputWidget);
}

InputWidget::~InputWidget() {
    _inputWidget->disconnect();
    _passwordWidget->disconnect();
    _inputWidget->deleteLater();
    _passwordWidget->deleteLater();
}


void InputWidget::toggleEchoMode(bool localEcho) {
    _localEcho = localEcho;
    _inputWidget->setEchoMode(_localEcho);
    if (_localEcho) {
        setFocusProxy(_inputWidget);
        setCurrentWidget(_inputWidget);
    }
    else {
        setFocusProxy(_passwordWidget);
        setCurrentWidget(_passwordWidget);
    }
}

void InputWidget::gotPasswordInput() {
    _passwordWidget->selectAll();
    QString input = _passwordWidget->text() + "\n";
    _passwordWidget->clear();

    emit sendUserInput(input.toLatin1());
}

void InputWidget::gotMultiLineInput(QString input) {
    QString str = input.append("\n");

    if (_localEcho) {
        emit displayMessage(str);
    }

    emit sendUserInput(str.toLatin1());
}

void InputWidget::relayMessage(const QString &message) {
    emit displayMessage(message);
}
