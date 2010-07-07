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

#ifndef _WINDOWACTIONMANAGER_H_
#define _WINDOWACTIONMANAGER_H_

#include <QMainWindow>

class MainWindow;
class QActionGroup;

class WindowActionManager: public QObject {
  Q_OBJECT

  public:
    static WindowActionManager* instance(MainWindow *parent = 0);
    void destroy();

    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;

public slots:
    void about();
    void clientHelp();
    void alwaysOnTop();
    void fullScreen();

    void openMumeWebsite();
    void openMumeForum();
    void openMumeWiki();
    void newbieHelp();

 protected:
    WindowActionManager(MainWindow*);
    ~WindowActionManager();
    
    static WindowActionManager* _pinstance;

  private:
    void disableActions(bool value);
    void postEvent(QVariant *payload, const QStringList& tags);

    MainWindow *_mainWindow;

    /* MainWindow (i.e. Normal) Actions */
    QAction *exitAct;
    QAction *clientHelpAct;

    QAction *profileAct;
    QAction *settingsAct;
    QAction *aliasAct;

    QAction *mumeWebsiteAct;
    QAction *mumeWikiAct;
    QAction *mumeForumAct;
    QAction *newbieHelpAct;

    QAction *aboutAct;
    QAction *aboutQtAct;

    QAction *alwaysOnTopAct;
    QAction *fullScreenAct;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QMenu *newbieMenu;
    QMenu *mumeMenu;
    QMenu *settingsMenu;

    QToolBar *editToolBar;
    QToolBar *connectToolBar;
};

#endif /* _WINDOWACTIONMANAGER_H_ */
