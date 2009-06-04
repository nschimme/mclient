#include "MClientDisplayPlugin.h"

#include <QEvent>


MClientDisplayPlugin::MClientDisplayPlugin(QWidget* parent) : MClientPlugin(parent) {
  _type = DISPLAY;
}


MClientDisplayPlugin::~MClientDisplayPlugin() {
}


void MClientDisplayPlugin::customEvent(QEvent* e) {
}


const bool MClientDisplayPlugin::initDisplay(QString s) {
}


QWidget* MClientDisplayPlugin::getWidget(QString s) {
}

