#include "MudCommandTask.h"

#include "MClientEvent.h"
#include <QDebug>

MudCommandTask::MudCommandTask(CommandProcessor *ps, QObject *parent)
  : AbstractCommandTask(ps, parent) {  
  _verbose = false;
  _socketOpen = false;
}


MudCommandTask::~MudCommandTask() {
}

void MudCommandTask::customEvent(QEvent *e) {
  if (e->type() == 10000) {
    // EngineEvent
    return ;

  }
  else if (e->type() == 10001) {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    if (me->dataTypes().contains("XMLAll")) {
      parseMudOutput(me->payload()->toString(),
		     me->dataTypes());
      
    } else
      AbstractCommandTask::customEvent(e);

  }
  else {
    qDebug() << "CommandTask" << QThread::currentThread()
	     << "received unknown event type:" << e->type();
  }
}
