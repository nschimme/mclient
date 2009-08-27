#include "UserCommandTask.h"

#include "MClientEvent.h"
#include <QDebug>

UserCommandTask::UserCommandTask(CommandProcessor *ps, QObject *parent)
  : AbstractCommandTask(ps, parent) {
  _socketOpen = false;
  _verbose = true;

}


UserCommandTask::~UserCommandTask() {
}

void UserCommandTask::customEvent(QEvent *e) {
  if (e->type() == 10000) {
    // EngineEvent
    return ;

  }
  else if (e->type() == 10001) {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    if (me->dataTypes().contains("UserInput")) {
      parseUserInput(me->payload()->toString());
      
    } else
      AbstractCommandTask::customEvent(e);

  }
  else {
    qDebug() << "CommandTask" << QThread::currentThread()
	     << "received unknown event type:" << e->type();
  }
}
