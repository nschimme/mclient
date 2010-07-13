#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientEventHandler.h"
#include <QPointer>

class TelnetParser;

class EventHandler : public MClientEventHandler {
    Q_OBJECT
    
    public:
         EventHandler(AbstractPluginSession *ps, MClientPlugin *mp);
        ~EventHandler();

        void customEvent(QEvent* e);

 public slots:
	/** toggles echo mode for passwords */
	void echoModeChanged(bool);

	/** Prepares data, doubles IACs, sends it using doSendData. */
	void socketWrite(const QByteArray &data);

	void displayData(const QString &data, bool goAhead);

 signals:	
	/** window size has changed - informs the server about it */
	void windowSizeChanged (int x, int y);

	void readData(const QByteArray &ba);

	void sendData(const QByteArray &data);

	void socketConnected();

	void socketDisconnected();

 private:
	TelnetParser *_telnetParser;
};


#endif /* EVENTHANDLER_H */
