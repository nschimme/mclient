/***************************************************************************
                          Telnet.h  -  handles telnet connection
    begin                : Pi Jun 14 2002
    copyright            : (C) 2002-2008 by Tomas Mecir
    email                : kmuddy@kmuddy.com

    This file has been modified for the mClient distribution from KMuddy.

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TELNET_H
#define TELNET_H

#include "MClientPlugin.h"
#include <QHash>

class AbstractPluginSession;
class EventHandler;

class Telnet : public MClientPlugin {
    Q_OBJECT
    
    public:
        Telnet(QObject* parent=0);
        ~Telnet();

        // From MClientFilterPlugin
        void configure();
        bool startSession(AbstractPluginSession *ps);
        bool stopSession(const QString &session);

	MClientEventHandler* getEventHandler(const QString &s);

 private:
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
};


#endif /* TELNET_H */
