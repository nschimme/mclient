#include "MClientFilterPlugin.h"

#include <QEvent>


MClientFilterPlugin::MClientFilterPlugin(QObject* parent) : MClientPlugin(parent) {
  _type = FILTER_PLUGIN;
}


MClientFilterPlugin::~MClientFilterPlugin() {
}


void MClientFilterPlugin::customEvent(QEvent* e) {
}
