#include "MClientDisplayPlugin.h"

#include <QEvent>


MClientDisplayPlugin::MClientDisplayPlugin(QWidget* parent) : MClientPlugin(parent) {
  _displayLocations = 0;
}


MClientDisplayPlugin::~MClientDisplayPlugin() {
}


void MClientDisplayPlugin::customEvent(QEvent*) {
}


const MClientDisplayLocations& MClientDisplayPlugin::displayLocations() const {
    return _displayLocations;
}


bool MClientDisplayPlugin::initDisplay(QString) {
  return false;
}


QWidget* MClientDisplayPlugin::getWidget(QString) {
  return 0;
}

