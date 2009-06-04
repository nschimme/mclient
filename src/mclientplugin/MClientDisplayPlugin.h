#ifndef MCLIENTDISPLAYPLUGIN_H
#define MCLIENTDISPLAYPLUGIN_H

#include "MClientDisplayInterface.h"
#include "MClientPlugin.h"
#include <QWidget>

class QEvent;


class MClientDisplayPlugin : public MClientPlugin,
        public MClientDisplayInterface {
    Q_OBJECT
    Q_INTERFACES(MClientDisplayInterface)
    
    public:
        MClientDisplayPlugin(QWidget* parent=0);
        ~MClientDisplayPlugin();

        // Plugin members
        virtual void customEvent(QEvent* e);

        // Display members
        virtual const bool initDisplay(QString s);
	virtual QWidget* getWidget(QString s);
};


#endif /* MCLIENTDISPLAYPLUGIN_H */
