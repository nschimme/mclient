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

#include <QtGui>
#include <QTextStream>
#include <QCloseEvent>
#include <QTabWidget>

#include "MainWindow.h"
#include "WindowActionManager.h"
#include "SmartSplitter.h"
#include "SmartTabWidget.h"
#include "SmartMenuBar.h"

#include "ConfigManager.h"
#include "PluginManager.h"
#include "PluginSession.h"

#include "MClientDefinitions.h"
#include "MClientPluginInterface.h"
#include "MClientEventHandler.h"
#include "MClientDisplayHandler.h"

#include "QuickConnectDialog.h"
#include "ProfileManagerDialog.h"
#include "AliasEditorDialog.h"

MainWindow::MainWindow(PluginManager *pm) {
  _pluginManager = pm;
  setWindowIcon(QIcon(":/mainwindow/m.png"));
  readSettings();
  
  /** Create Other Child Widgets */
  delete menuBar();
  setMenuBar(new SmartMenuBar(this));
  // Initialize WindowActionManager
  WindowActionManager *actMgr = WindowActionManager::instance(this);
  actMgr->createActions();
  actMgr->createMenus();
  actMgr->createToolBars();
  actMgr->createStatusBar();

  /** Create Primary Display Widgets */
  _tabWidget = new SmartTabWidget;
  setCentralWidget(_tabWidget);

  /** Create Splash Screen */
  _splash = new QSplashScreen(QPixmap(":/mainwindow/intro.png"));
  _splash->setAttribute(Qt::WA_DeleteOnClose);
  _splash->showMessage("Loading configuration...",
		       Qt::AlignBottom | Qt::AlignHCenter,
		       Qt::white);
  _splash->show();
  _splash->finish(this);
    
  qDebug() << "MainWindow created with thread:" << this->thread();
}


MainWindow::~MainWindow() {
  getPluginManager()->getConfig()->~ConfigManager();
  getPluginManager()->~PluginManager();
  qDebug() << "* MainWindow destroyed";
}


void MainWindow::start() {
  bool showQuickConnect = true;

  // Profile Command Line support
  QStringList args = QCoreApplication::arguments();
  if (args.contains("-profile")) {
    int pos = args.indexOf("-profile") + 1;
    if (pos < args.size()) {
      QString profile = args.at(pos);
      QStringList profileNames
	= getPluginManager()->getConfig()->profileNames();
      if (profileNames.contains(profile)) {
	startProfile(profile);
	showQuickConnect = false;
      }
      else
	qDebug() << "Unknown profile" << profile;
      
    }
  }

  // Quick Connect Dialog
  if (showQuickConnect) {
    _quickConnectDlg = new QuickConnectDialog(getPluginManager()->getConfig());
    
    connect(_quickConnectDlg, SIGNAL(profileSelected(const QString&)),
	    this, SLOT(startProfile(const QString&)));
    
    _quickConnectDlg->show();
  }

}


void MainWindow::startProfile(const QString &profile) {
  qDebug() << "* starting profile" << profile;

  if (_quickConnectDlg) {
    _quickConnectDlg->close();
    delete _quickConnectDlg;
  }

  /** Create Second Splash Screen */
  _splash = new QSplashScreen(QPixmap(":/mainwindow/intro.png"));
  _splash->setAttribute(Qt::WA_DeleteOnClose);
  _splash->showMessage("Loading plugins...",
		       Qt::AlignBottom | Qt::AlignHCenter,
		       Qt::white);
  _splash->show();
  _splash->finish(this);

  setCurrentProfile(profile);
  emit startSession(profile);
}


void MainWindow::initDisplay(PluginSession *ps) {
  // Create menus and generate a widget list
  QList<QPair<int, QWidget*> > widgetList;

  QString session(ps->session());

  qDebug() << "* Initializing display widgets from thread"
	   << this->thread() << "for session" << session;
  // We now start/create the widgets (this is a slot from
  // PluginSession) in the main thread.

  foreach(QPluginLoader* pl, ps->loadedPlugins()) {
    MClientPluginInterface* pi
      = qobject_cast<MClientPluginInterface*>(pl->instance());
    if (pi) {

      MClientEventHandler *eh
	= qobject_cast<MClientEventHandler*>(pi->getEventHandler(session));

      if (eh) {
	// Create menus
	MenuData menus = eh->createMenus();
	if (!menus.isEmpty()) {
	  qDebug() << "* Display found menus for plugin"
		   << pi->shortName() << menus;
	  
	  static_cast<SmartMenuBar*>(menuBar())->addMenu(menus);
	}

	// Create the widgets outside of the threads
	MClientDisplayHandler *dh
	  = qobject_cast<MClientDisplayHandler*>(eh);
	if (dh) {
	  // Initialize the display
	  QPair<int, QWidget*> pair(dh->displayLocations(),
				    dh->createWidget());
	  widgetList << pair;
	  
	}

      }
    }
  }
  qDebug() << "* received widgets" << widgetList;

  // Generate menus
  static_cast<SmartMenuBar*>(menuBar())->generateMenus();

  // Create the layout
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);

  QPointer<QWidget> display, input;
  bool displaySet, inputSet = false;
  for (int i = 0; i < widgetList.size(); ++i) {
    int position = widgetList.at(i).first;

    qDebug() << "* looking at" << widgetList.at(i).second;
    // Differentiate between the types
    if (ISSET(position, DL_DISPLAY) && !displaySet) {
      // Primary Display Widget
      display = widgetList.at(i).second;
      displaySet = true;

    } else if (ISSET(position, DL_INPUT) && !inputSet) {
      // Primary Input Widget
      input = widgetList.at(i).second;
      inputSet = true;

    } else {
      // Display the widget if it floats (or is unsupported)
      QDockWidget *dockWidget = new QDockWidget(this);
      dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
      dockWidget->setFeatures(QDockWidget::DockWidgetMovable |
			      QDockWidget::DockWidgetFloatable |
			      QDockWidget::DockWidgetClosable);
      dockWidget->setWidget(widgetList.at(i).second);
      dockWidget->setFloating(false);
      addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
      _dockWidgets.insert("test", dockWidget);
      //widgetList.at(i).second->show();
      
    }

  }
  qDebug() << "* MainWindow has collected all widgets, displaying them...";

  // Add the primary widgets to the smart splitter
  // TODO: Make it smart and resizable upon request of the widget!
  SmartSplitter *_splitter = new SmartSplitter(Qt::Vertical);
  _tabWidget->addTab(_splitter, _currentProfile);

  // Add the widgets
  if (displaySet) {
    qDebug() << "* display is" << display;
    _splitter->addWidget(display);
    _splitter->setCollapsible(_splitter->indexOf(display), false);
  }
  if (inputSet) {
    qDebug() << "* input is" << input;
    _splitter->addWidget(input);
    _splitter->setCollapsible(_splitter->indexOf(input), false);
  }

  // Display Main Window
  qDebug() << "* Displaying Main Window";
  show();

  // Connect the signals/slots
  if (displaySet && inputSet) {
    display->setFocusProxy(input);
    input->setFocus();
  }

}

void MainWindow::closeEvent(QCloseEvent *event)
{
  qDebug() << "MainWindow received closeEvent";
  if (maybeSave()) {
    emit stopSession(currentSession());
    writeSettings();
    deleteLater();
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::readSettings()
{
  resize(QSize(640, 480));
  //move(QPoint(200, 200));
}

void MainWindow::writeSettings()
{
  _pluginManager->getConfig()->writeApplicationSettings();
  /*
  Config().setWindowPosition(pos() );
  Config().setWindowSize(size() );
  Config().setWindowState(saveState() );
  Config().setAlwaysOnTop((bool)(windowFlags() &
  Qt::WindowStaysOnTopHint));
  */
}

bool MainWindow::maybeSave()
{
  /*
  if (Config().isChanged()) {
    int ret = QMessageBox::warning(this, tr("mClient"),
                                   tr("Your client settings have been modified.\n"
                                       "Do you want to save your changes?"),
                                       QMessageBox::Yes | QMessageBox::Default,
                                       QMessageBox::No,
                                       QMessageBox::Cancel | QMessageBox::Escape);
    if (ret == QMessageBox::Yes)
      Config().write();
    else if (ret == QMessageBox::Cancel)
      return false;
  }
  */
  return true;
}

void MainWindow::setCurrentProfile(const QString &session)
{
  _currentProfile = session;
  setWindowModified(false);

  QString shownName;
  if (_currentProfile.isEmpty())
    shownName = "No profile";
  else
    shownName = _currentProfile;

  setWindowTitle(tr("%1 - mClient").arg(shownName));
}


void MainWindow::changeConfiguration() {
  getPluginManager()->configure();
}

void MainWindow::manageProfiles() {
  ProfileManagerDialog *profileManager
    = new ProfileManagerDialog(getPluginManager()->getConfig(), 0, this);

  // Connect the load signal
  connect(profileManager, SIGNAL(loadProfile(const QString&)),
	  this, SLOT(startProfile(const QString&)));

  profileManager->exec();
  delete profileManager;
}

void MainWindow::aliasEditor() {
  AliasEditorDialog *aliasEditor
    = new AliasEditorDialog(getPluginManager()->
			    getPluginSession(_currentProfile)->
			    getAlias(), this);
  aliasEditor->exec();
  delete aliasEditor;
}

const QString MainWindow::currentSession() const {
  return _tabWidget->tabText(_tabWidget->currentIndex());
}
