#ifndef USERCOMMANDTASK_H
#define USERCOMMANDTASK_H

#include "AbstractCommandTask.h"

class UserCommandTask : public AbstractCommandTask {
    
    public:
        UserCommandTask(CommandProcessor*, QObject *parent=0);
        ~UserCommandTask();

	void customEvent(QEvent *e);
};


#endif /* USERCOMMANDTASK_H */
