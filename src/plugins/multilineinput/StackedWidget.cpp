#include "StackedWidget.h"

#include "EventHandler.h"
#include "InputWidget.h"
#include "PasswordWidget.h"
#include <QDebug>

StackedWidget::StackedWidget(EventHandler *eh, QWidget* parent) 
  : QStackedWidget(parent) {

  _inputWidget = new InputWidget;
  _passwordWidget = new PasswordWidget;
  addWidget(_inputWidget);
  addWidget(_passwordWidget);
  setFocusProxy(_inputWidget);

  // EventHandler -> StackedWidget
  connect(eh, SIGNAL(setEchoMode(bool)),
	  SLOT(toggleEchoMode(bool)));
  
  // EventHandler <-> Input Widget
  connect(_inputWidget, SIGNAL(sendUserInput(const QString&, bool)),
	  eh, SLOT(sendUserInput(const QString&, bool)));
  connect(_inputWidget, SIGNAL(displayMessage(const QString &)),
	  eh, SLOT(displayMessage(const QString &)));
  connect(eh, SIGNAL(showCommandHistory()),
	  _inputWidget, SLOT(showCommandHistory()));
  connect(eh, SIGNAL(addTabHistory(const QStringList &)),
	  _inputWidget, SLOT(addTabHistory(const QStringList &)));
  
  // Password Widget -> EventHandler
  connect(_passwordWidget, SIGNAL(sendUserInput(const QString&, bool)),
	  eh, SLOT(sendUserInput(const QString&, bool)));
}


StackedWidget::~StackedWidget() {
  _inputWidget->disconnect();
  _passwordWidget->disconnect();
  _inputWidget->deleteLater();
  _passwordWidget->deleteLater();
}


void StackedWidget::toggleEchoMode(bool localEcho) {
  if (localEcho) {
    setFocusProxy(_inputWidget);
    setCurrentWidget(_inputWidget);
  }
  else {
    setFocusProxy(_passwordWidget);
    setCurrentWidget(_passwordWidget);
  }
}
