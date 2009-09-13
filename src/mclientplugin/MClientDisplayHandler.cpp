#include "MClientDisplayHandler.h"

#include <QEvent>

MClientDisplayHandler::MClientDisplayHandler(PluginSession *ps,
					     MClientPlugin *mp)
  : MClientEventHandler(ps, mp) {
  _displayLocations = 0;
}


MClientDisplayHandler::~MClientDisplayHandler() {
}


void MClientDisplayHandler::customEvent(QEvent*) {
}


const MClientDisplayLocations& MClientDisplayHandler::displayLocations() const {
    return _displayLocations;
}


QWidget* MClientDisplayHandler::createWidget() {
  return 0;
}
