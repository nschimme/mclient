#ifndef MCLIENTDISPLAYHANDLER_H
#define MCLIENTDISPLAYHANDLER_H

#include "MClientDisplayHandlerInterface.h"
#include "MClientEventHandler.h"
#include <QWidget>

class PluginSession;
class MClientPlugin;
class QEvent;

class MClientDisplayHandler : public MClientEventHandler, public MClientDisplayHandlerInterface {
    Q_OBJECT
    Q_INTERFACES(MClientDisplayHandlerInterface)

    public:
        MClientDisplayHandler(PluginSession *ps, MClientPlugin *mp);
        virtual ~MClientDisplayHandler();

        // Plugin members
        virtual void customEvent(QEvent* e);

        // The field of possible display locations
        const MClientDisplayLocations& displayLocations() const;

        // Display members
	virtual QWidget* createWidget();

   protected:
	MClientDisplayLocations _displayLocations;

};

#endif /* MCLIENTDISPLAYHANDLER_H */
