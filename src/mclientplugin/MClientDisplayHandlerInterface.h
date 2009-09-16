#ifndef MCLIENTDISPLAYHANDLERINTERFACE_H
#define MCLIENTDISPLAYHANDLERINTERFACE_H

#include <QtPlugin>
#include <QWidget>

typedef quint8 MClientDisplayLocations;
#define DL_DISPLAY    bit1 // Primary Display (Center Layout)
#define DL_INPUT      bit2 // Primary Input   (Bottom Layout)
#define DL_LEFT       bit3 // Dock Left
#define DL_RIGHT      bit4 // Dock Right
#define DL_BOTTOM     bit5 // Dock Bottom
#define DL_TOP        bit6 // Dock Top
#define DL_FLOAT      bit7 // Floating	     

class MClientDisplayHandlerInterface {
    
    public:
        // The field of possible display locations
        virtual const MClientDisplayLocations& displayLocations() const=0;

        // Display members
	virtual QWidget* createWidget()=0;

};

Q_DECLARE_INTERFACE(MClientDisplayHandlerInterface,
        "mume.mclient.MClientDisplayHandlerInterface/1.0")

#endif // MCLIENTDISPLAYHANDLERINTERFACE_H
