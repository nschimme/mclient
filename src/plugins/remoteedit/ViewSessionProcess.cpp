/***************************************************************************
 *   Copyright (C) 2008 by Nils Schimmelmann   *
 *   nschimme@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 ***************************************************************************/

#include "ViewSessionProcess.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QTemporaryFile>

ViewSessionProcess::ViewSessionProcess(int key, const QByteArray &title,
				       const QByteArray &body, QObject *parent)
  : QProcess(parent), _key(key), _title(title), _body(body) {
  
  setReadChannelMode(QProcess::MergedChannels);

  // Signals/Slots
  connect(this, SIGNAL(finished(int, QProcess::ExitStatus)),
	  SLOT(onFinished(int, QProcess::ExitStatus)));
  connect(this, SIGNAL(error(QProcess::ProcessError)),
	  SLOT(onError(QProcess::ProcessError)));

  QString keyTemp;
  if (key == -1) keyTemp = "view";
  else keyTemp = QString("key%1").arg(_key);

  // Set the file template
  QString fileTemplate = QString("%1mClient.%2.pid%3.XXXXXX")
    .arg(QDir::tempPath() + QDir::separator()) // %1
    .arg(keyTemp)                              // %2
    .arg(QCoreApplication::applicationPid());  // %3
  _file.setFileTemplate(fileTemplate);

  // Try opening up the temporary file
  if (_file.open()) {
    const QString &fileName = _file.fileName();
    qDebug() << "* View session file template" << fileName;
    _file.write(_body);
    _file.flush();
    _file.close();

    // Well, this might not hurt, might as well attempt it
    putenv(QString("TITLE=%1" + _title).toAscii().data());

    // Start the process!
    QStringList args;
#ifdef Q_WS_WIN
    args << fileName;
    start("notepad", args);
#else
    args << "-e" << "nano" << fileName;
    start("xterm", args);
#endif

    qDebug() << "* View session" << key << title << "started";

  }
  else {
    qCritical() << "! View session was unable to create a temporary file";
    onError(QProcess::FailedToStart);
  }
}


ViewSessionProcess::~ViewSessionProcess() {
}


void ViewSessionProcess::onFinished(int /*exitCode*/,
				    QProcess::ExitStatus /*status*/) {
  deleteLater();
}


void ViewSessionProcess::onError(QProcess::ProcessError /*error*/) {
  qWarning() << "! View session" << _key << "encountered an error:"
	     << errorString();
  qWarning() << "Output:" << readAll();
  deleteLater();
}
