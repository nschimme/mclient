#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientEventHandler.h"

class QEvent;
class ScriptEngine;

class EventHandler : public MClientEventHandler {
    Q_OBJECT
    
    public:
        EventHandler(AbstractPluginSession *ps, MClientPlugin *mp);
        ~EventHandler();

        void customEvent(QEvent* e);

public slots:
        void parseInput(const QString &input);

    private:
	ScriptEngine *_scriptEngine;
	
 signals:
	void evaluate(const QString&);
	void variable(const QString&);
};


#endif /* EVENTHANDLER_H */
