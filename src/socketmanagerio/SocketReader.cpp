#include "SocketReader.h"
#include "SocketManagerIO.h"

#include <QDebug>
#include <QString>
#include <QTcpSocket>


SocketReader::SocketReader(QString s, SocketManagerIO* sm, QObject* parent) 
    : QThread(parent) { 
   
    _session = s;
    _sm = sm;

    _socket = new QTcpSocket(this);
    _proxy.setType(QNetworkProxy::NoProxy);
    _socket->setProxy(_proxy);
    
    connect(_socket, SIGNAL(connected()), this, SLOT(on_connect())); 
    connect(_socket, SIGNAL(disconnected()), this, SLOT(on_disconnect())); 
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), 
            this, SLOT(on_error())); 
    connect(_socket, SIGNAL(readyRead()), this, SLOT(on_readyRead())); 
}


void SocketReader::connectToHost() {//const QString host, const int& port) {
    //_host = host;
    //_ports = port;
    //FIXME: HACK! :(
    if(_socket->thread() != this->thread()) {
        qDebug() << "* threads in SocketReader:" << this->thread() 
            << _socket->thread();
        _socket->moveToThread(this->thread());
    }

    _sm->displayMessage(QString("#trying %1:%2... ").arg(_host).arg(_port),
			_session);

    _socket->connectToHost(_host, _port);
}


SocketReader::~SocketReader() {
    terminate();
    wait();
    delete _socket;
}


void SocketReader::on_connect() {
    _sm->displayMessage(QString("connected!\n"), _session);
    _sm->socketOpened(this);
}


void SocketReader::on_readyRead() {
    if(!isRunning()) start(LowPriority);
}


void SocketReader::on_disconnect() {
    _sm->displayMessage(QString("#connection on \"%1\" closed.\n").arg(_session), _session);
    _sm->socketClosed(this);
}


void SocketReader::on_error() {
    qWarning() << "Error involving" 
       << _host << _port << _socket->errorString();
    _sm->displayMessage(_socket->errorString().append("\n"), _session);
}


const int& SocketReader::port() const {
    return _port;
}


void SocketReader::closeSocket() const {
    _socket->close();
}


void SocketReader::run() {
    setTerminationEnabled(false);
    
    qDebug() << "SocketReader" << _session << "reading data!";
    // Just read everything there is to be read into a QByteArray.
    QByteArray ba = _socket->readAll(); 
    _sm->socketReadData(ba, _session);
    
    setTerminationEnabled(true);
}


void SocketReader::sendToSocket(const QByteArray* ba) {
    if(_socket->state() != QAbstractSocket::ConnectedState) return;
    qDebug() << "Got user input:" << ba->data() << "\t" << ba->length();
    //str << str.size();
    //int len = _socket->write(ba.toLatin1().data(), str.size());
    //int len = _socket->write(*ba);

    const char *dd = ba->data();
    int dataLength = ba->length();

    _socket->write(ba->data());

//     int written = 0;
//     do {
//       int w = _socket->write(dd + written, dataLength - written);
//       // TODO: need some error diagnostics
//       if (w == -1)  // buffer full - try again
// 	continue;
//       written += w;
//     } while (written < dataLength);

    qDebug() << dataLength << "bytes written";
    delete ba;
}


const QString& SocketReader::session() const {
    return _session;
}


void SocketReader::host(const QString host) {
    _host = host;
}


void SocketReader::port(const int port) {
    _port = port;
}


void SocketReader::proxy(const QNetworkProxy* proxy) {
    _proxy = *proxy;
    _socket->setProxy(_proxy);
}
