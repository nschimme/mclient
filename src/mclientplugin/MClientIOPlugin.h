#ifndef MCLIENTIOPLUGIN_H
#define MCLIENTIOPLUGIN_H

#include "MClientPlugin.h"
#include "MClientIOInterface.h"

class QByteArray;
class QEvent;


class MClientIOPlugin : public MClientPlugin, public MClientIOInterface {
    Q_OBJECT
    Q_INTERFACES(MClientIOInterface)
    
    public:
        MClientIOPlugin(QObject* parent=0);
        ~MClientIOPlugin();

        // Plugin members
        virtual void customEvent(QEvent* e);

        // IO members
        virtual void sendData(const QByteArray data);
        virtual void connectDevice(QString s);
        virtual void disconnectDevice(QString s);

};


#endif /* MCLIENTIOPLUGIN_H */
