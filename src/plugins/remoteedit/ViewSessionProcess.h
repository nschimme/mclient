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
 
#ifndef _VIEWSESSIONPROCESS_H_
#define _VIEWSESSIONPROCESS_H_

#include <QProcess>
#include <QTemporaryFile>

class ViewSessionProcess: public QProcess {
  Q_OBJECT

  public:
    ViewSessionProcess(int key, const QByteArray &title,
		       const QByteArray &body, QObject *parent=0);
    virtual ~ViewSessionProcess();

private slots:
    virtual void onError(QProcess::ProcessError);
    virtual void onFinished(int, QProcess::ExitStatus);

  protected:
    int _key;
    QByteArray _title;
    QByteArray _body;
    
    QTemporaryFile _file;

};

#endif /* _VIEWSESSIONPROCESS_H_ */
