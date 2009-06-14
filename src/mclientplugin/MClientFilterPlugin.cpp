#include "MClientFilterPlugin.h"

#include <QEvent>


MClientFilterPlugin::MClientFilterPlugin(QObject* parent) : MClientPlugin(parent) {
}


MClientFilterPlugin::~MClientFilterPlugin() {
}


void MClientFilterPlugin::customEvent(QEvent* e) {
}
