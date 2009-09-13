#ifndef MCLIENTDISPLAYHANDLER_H
#define MCLIENTDISPLAYHANDLER_H

#include "MClientEventHandler.h"
#include "MClientDefinitions.h"
#include <QWidget>

class PluginSession;
class MClientPlugin;
class QEvent;

class MClientDisplayHandler : public MClientEventHandler {
    Q_OBJECT

    public:
        MClientDisplayHandler(PluginSession *ps, MClientPlugin *mp);
        ~MClientDisplayHandler();

        // Plugin members
        virtual void customEvent(QEvent* e);

        // The field of possible display locations
        virtual const MClientDisplayLocations& displayLocations() const;

        // Display members
	virtual QWidget* createWidget();

   protected:
	MClientDisplayLocations _displayLocations;

};

#endif /* MCLIENTDISPLAYHANDLER_H */
