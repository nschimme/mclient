#include "MClientIOPlugin.h"

#include <QEvent>


MClientIOPlugin::MClientIOPlugin(QObject* parent) : MClientPlugin(parent) {
}


MClientIOPlugin::~MClientIOPlugin() {
}


void MClientIOPlugin::customEvent(QEvent*) {
    // What we'll most likely do is check the event and call send data on its
    // contents.
}


void MClientIOPlugin::sendData(const QByteArray) {
    // Send the data to an external source here.
}


void MClientIOPlugin::connectDevice(QString) {
}


void MClientIOPlugin::disconnectDevice(QString) {
}
