#ifndef MCLIENTDISPLAYHANDLER_H
#define MCLIENTDISPLAYHANDLER_H

#include "MClientEventHandler.h"
#include <QWidget>

class AbstractPluginSession;
class MClientPlugin;
class QEvent;

typedef quint8 MClientDisplayLocations;
#define DL_DISPLAY    bit1 // Primary Display (Center Layout)
#define DL_INPUT      bit2 // Primary Input   (Bottom Layout)
#define DL_LEFT       bit3 // Dock Left
#define DL_RIGHT      bit4 // Dock Right
#define DL_BOTTOM     bit5 // Dock Bottom
#define DL_TOP        bit6 // Dock Top
#define DL_FLOAT      bit7 // Floating

class MClientDisplayHandler : public MClientEventHandler {
    Q_OBJECT

    public:
        MClientDisplayHandler(AbstractPluginSession *ps, MClientPlugin *mp);
        virtual ~MClientDisplayHandler();

        // Plugin members
        virtual void customEvent(QEvent* e);

        // The field of possible display locations
        const MClientDisplayLocations& displayLocations() const;

        // Display members
	virtual QWidget* createWidget(QWidget *parent=0);

   protected:
	MClientDisplayLocations _displayLocations;

};

#endif /* MCLIENTDISPLAYHANDLER_H */
