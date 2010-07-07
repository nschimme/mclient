#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientEventHandler.h"

class QEvent;
class QTimer;

class EventHandler : public MClientEventHandler {
    Q_OBJECT
    
    public:
         EventHandler(AbstractPluginSession *ps, MClientPlugin *mp);
        ~EventHandler();

        void customEvent(QEvent* e);

public slots:
        void sendToSocket(const QByteArray &);

   private:
	int _key;
	QByteArray _title;
	QByteArray _body;
	
	enum EditState {NORMAL, RECEIVED_KEY, RECEIVED_TITLE};
	EditState _state;

	void editSession(int key, const QByteArray &title,
			 const QByteArray &body);
	void viewSession(int key, const QByteArray &title,
			 const QByteArray &body);
	void sendMUMEIdentifyRequest();

};


#endif /* EVENTHANDLER_H */
