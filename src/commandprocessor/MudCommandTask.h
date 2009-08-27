#ifndef MUDCOMMANDTASK_H
#define MUDCOMMANDTASK_H

#include "AbstractCommandTask.h"

class MudCommandTask : public AbstractCommandTask {
    
    public:
        MudCommandTask(CommandProcessor*, QObject *parent=0);
        ~MudCommandTask();

	void customEvent(QEvent *e);
};


#endif /* MUDCOMMANDTASK_H */
