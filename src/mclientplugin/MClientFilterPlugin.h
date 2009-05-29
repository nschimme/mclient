#ifndef MCLIENTFILTERPLUGIN_H
#define MCLIENTFILTERPLUGIN_H

#include "MClientFilterInterface.h"
#include "MClientPlugin.h"

class QEvent;


class MClientFilterPlugin : public MClientPlugin,
        public MClientFilterInterface {
    Q_OBJECT
    Q_INTERFACES(MClientFilterInterface)
    
    public:
        MClientFilterPlugin(QObject* parent=0);
        ~MClientFilterPlugin();

        // Plugin members
        virtual void customEvent(QEvent* e);
};


#endif /* MCLIENTFILTERPLUGIN_H */
