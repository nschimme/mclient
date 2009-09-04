#ifndef MCLIENTDISPLAYHANDLER_H
#define MCLIENTDISPLAYHANDLER_H

#include "MClientEventHandler.h"
#include "MClientDefinitions.h"
#include <QWidget>

class QEvent;

class MClientDisplayHandler : public MClientEventHandler {
    Q_OBJECT

    public:
        MClientDisplayHandler(QWidget* parent=0);
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
