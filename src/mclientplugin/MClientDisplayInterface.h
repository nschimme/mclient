#ifndef MCLIENTDISPLAYINTERFACE_H
#define MCLIENTDISPLAYINTERFACE_H

#include <QtPlugin>

#include "MClientDefinitions.h"

class MClientDisplayInterface {
    
    public:
        // called from the main thread; initialises all QWidgets needed.
        // The plugin developer MUST do this, or the plugin will cause a
        // segfault.  (actually I'm not positive that's such a strong
        // 'must')

	// The field with available display locations
	virtual const MClientDisplayLocations& displayLocations() const=0;

	// Display functions
        virtual const bool initDisplay(QString s)=0;
	virtual QWidget* getWidget(QString s)=0;

};

Q_DECLARE_INTERFACE(MClientDisplayInterface,
        "mume.mclient.MClientDisplayInterface/1.0")

#endif /* MCLIENTDISPLAYINTERFACE_H */
