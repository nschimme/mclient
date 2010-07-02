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

#include "EditSessionProcess.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QTemporaryFile>
#include <QFileInfo>

EditSessionProcess::EditSessionProcess(int key, const QByteArray &title,
				       const QByteArray &body, QObject *parent)
  : ViewSessionProcess(key, title, body, parent) {
  // Store the file information
  QFileInfo fileInfo(_file);
  _previousTime = fileInfo.lastModified();
}


EditSessionProcess::~EditSessionProcess() {
  qDebug() << "* Edit session" << _key << "was destroyed";
}


void EditSessionProcess::onFinished(int exitCode,
				    QProcess::ExitStatus status) {
  qDebug() << "* Edit session" << _key << "process finished with code"
	   << exitCode;
  if (status == QProcess::NormalExit) {
    // See if the file was modified since we created it
    QFileInfo fileInfo(_file);
    QDateTime currentTime = fileInfo.lastModified();
    if (_previousTime != currentTime) {
      // Read the file and submit it to MUME
      qDebug() << "* Edit session" << _key << "had changes, reading";
      if (_file.open())
	_body = _file.readAll();
      else
	qWarning() << "! Edit session" << _key << "unable to read file!";
      return finishEdit();

    }
    else qDebug() << "* Edit session" << _key << "canceled (no changes)";

  }
  else qWarning() << "! File process did not end normally";

  // Cancel
  cancelEdit();
}

void EditSessionProcess::onError(QProcess::ProcessError /*error*/) {
  qWarning() << "! Edit session" << _key << "encountered an error:"
	     << errorString();
  qWarning() << "Output:" << readAll();
  cancelEdit();
}


void EditSessionProcess::cancelEdit() {
  // Do the whacky MUME remote editing protocol
  const QString &keystr = QString("C%1\n").arg(_key);
  const QString &buffer = QString("%1E%2\n%3")
    .arg(MPI)
    .arg(keystr.length())
    .arg(keystr);
  qDebug() << "* Edit session" << _key << "writing to socket:" << buffer;
  emit sendToSocket(buffer.toAscii());
  deleteLater();
}


void EditSessionProcess::finishEdit() {
  // The contents have to be followed by a LF if they are not empty
  if (!_body.isEmpty())
    if (_body.at(_body.length()-1) != '\n')
      _body.append('\n');

  // Do the whacky MUME remote editing protocol
  const QString &keystr = QString("E%1\n").arg(_key);
  const QString &buffer = QString("%1E%2\n%3")
    .arg(MPI)
    .arg(_body.length() + keystr.length())
    .arg(keystr);
  _body.prepend(buffer.toAscii());

  qDebug() << "* Edit session" << _key << "writing to socket:" << _body;
  emit sendToSocket(_body);
  deleteLater();
}
