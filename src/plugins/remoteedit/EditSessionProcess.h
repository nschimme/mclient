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
 
#ifndef _EDITSESSIONPROCESS_H_
#define _EDITSESSIONPROCESS_H_

#include "ViewSessionProcess.h"
#include <QFileInfo>

#define MPI "~$#E" // MUME protocol introducer

class EditSessionProcess: public ViewSessionProcess {
  Q_OBJECT

  public:
    EditSessionProcess(int key, const QByteArray &title,
		       const QByteArray &body, QObject *parent=0);
    ~EditSessionProcess();

private slots:
    void onError(QProcess::ProcessError);
    void onFinished(int, QProcess::ExitStatus);

 protected:
    void cancelEdit();
    void finishEdit();

  private:
    QFileInfo _fileInfo;

 signals:
    void sendToSocket(int, const QByteArray &);

};

#endif /* _EDITSESSIONPROCESS_H_ */
