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
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef MainWindow_H
#define MainWindow_H

#include <QtGui>
#include <QPointer>

class QAction;
class QMenu;
class PluginManager;
class QuickConnectDialog;

class MainWindow : public QMainWindow {
  Q_OBJECT

    public:
  MainWindow(PluginManager *pm);
  ~MainWindow();
  
  const QString currentSession() const;

  PluginManager* getPluginManager() { return _pluginManager; }

public slots:
    void start();
    void receiveWidgets(const QList< QPair<int, QWidget*> >&);
    void startProfile(const QString&);

private slots:
    void changeConfiguration();
    void manageProfiles();
    void aliasEditor();

    void setCurrentProfile(const QString &profile);

  protected:   
    void closeEvent(QCloseEvent *event);

  private:
    QString _currentProfile;
    QTabWidget *_tabWidget;
    QHash<QString, QDockWidget*> _dockWidgets;
    PluginManager *_pluginManager;

    QPointer<QuickConnectDialog> _quickConnectDlg;
    QPointer<QSplashScreen> _splash;

    void readSettings();
    void writeSettings();
    bool maybeSave();

   signals:
    void startSession(const QString &s);
    void stopSession(const QString &s);

};

#endif
