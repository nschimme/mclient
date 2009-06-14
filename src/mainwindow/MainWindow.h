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

class QAction;
class QMenu;
class PluginManager;

class MainWindow:public QMainWindow
{
  Q_OBJECT

  public:
  // Singleton methods
  static MainWindow* instance(PluginManager *pm=0);
    ~MainWindow();

  const QString& session() const { return _currentProfile; }

  PluginManager* getPluginManager() { return _pluginManager; }

public slots:
    void start();
    void receiveWidgets(const QList< QPair<int, QWidget*> >&);

private slots:
    void changeConfiguration();

    void setCurrentProfile(const QString &profile);

  protected:
    // It's a singleton, so these go here
    MainWindow(PluginManager *pm);
    
    static MainWindow* _pinstance;
    
    void closeEvent(QCloseEvent *event);

  private:
    QString _currentProfile;
    QTabWidget *_tabWidget;
    QBoxLayout *_layout;
    QHash<QString, QDockWidget*> _dockWidgets;
    PluginManager *_pluginManager;

    void readSettings();
    void writeSettings();
    bool maybeSave();

   signals:
    void startSession(const QString &s);
    void stopSession(const QString &s);

};

#endif
