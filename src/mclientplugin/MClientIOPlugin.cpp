#include "MClientIOPlugin.h"

#include <QEvent>


MClientIOPlugin::MClientIOPlugin(QObject* parent) : MClientPlugin(parent) {
}


MClientIOPlugin::~MClientIOPlugin() {
}


void MClientIOPlugin::customEvent(QEvent* e) {
    // What we'll most likely do is check the event and call send data on its
    // contents.
}


void MClientIOPlugin::sendData(const QByteArray data) {
    // Send the data to an external source here.
}


void MClientIOPlugin::connectDevice(QString s) {
}


void MClientIOPlugin::disconnectDevice(QString s) {
}
