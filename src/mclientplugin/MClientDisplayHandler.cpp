#include "MClientDisplayHandler.h"

#include <QEvent>

MClientDisplayHandler::MClientDisplayHandler(QWidget* parent)
  : MClientEventHandler(parent) {
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
