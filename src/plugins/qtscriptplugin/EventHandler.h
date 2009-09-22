#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientCommandHandler.h"

class QEvent;
class ScriptEngine;

class EventHandler : public MClientCommandHandler {
    Q_OBJECT
    
    public:
        EventHandler(PluginSession *ps, MClientPlugin *mp,
		     CommandTask *task);
        ~EventHandler();

        void customEvent(QEvent* e);
        void parseInput(const QString &input);
	void init();

public slots:
        void postCommand(const QString &input);

    private:
	ScriptEngine *_scriptEngine;
	
 signals:
	void evaluate(const QString&);
	void variable(const QString&);
};


#endif /* EVENTHANDLER_H */
