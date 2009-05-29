#ifndef MCLIENTIOINTERFACE_H
#define MCLIENTIOINTERFACE_H

#include <QtPlugin>


class MClientIOInterface {
    
    public:

        // IO means we're sending data outside the program, or we're
        // receiving it from some external source.  Both can be done with
        // streams of bytes, which would be char* in C.  QByteArray works.
        virtual void sendData(const QByteArray data)=0;

        // This one is private, and thus is not part of the interface.
        //virtual const QByteArray receiveData() const=0;
        
        // Connects to something external (or perhaps internal) in an
        // implementation-specific way, getting all options from the object.
        virtual void connectDevice(QString s)=0;
        virtual void disconnectDevice(QString s)=0;


};

Q_DECLARE_INTERFACE(MClientIOInterface,
        "mume.mclient.MClientIOInterface/1.0")

#endif /* MCLIENTIOINTERFACE_H */
