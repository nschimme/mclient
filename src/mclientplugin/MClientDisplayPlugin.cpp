#include "MClientDisplayPlugin.h"

#include <QEvent>


MClientDisplayPlugin::MClientDisplayPlugin(QWidget* parent) : MClientPlugin(parent) {
  _type = DISPLAY;
  _displayLocations = 0;
}


MClientDisplayPlugin::~MClientDisplayPlugin() {
}


void MClientDisplayPlugin::customEvent(QEvent* e) {
}


const MClientDisplayLocations& MClientDisplayPlugin::displayLocations() const {
    return _displayLocations;
}


const bool MClientDisplayPlugin::initDisplay(QString s) {
  return false;
}


QWidget* MClientDisplayPlugin::getWidget(QString s) {
  return 0;
}

