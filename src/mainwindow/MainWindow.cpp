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
#include "ActionManager.h"

#include "ConfigManager.h"
#include "PluginManager.h"
#include "CommandManager.h"

#include "MClientDefinitions.h"

MainWindow* MainWindow::_pinstance = 0;

MainWindow* MainWindow::instance() {
    if(!_pinstance) {
        _pinstance = new MainWindow();
    }

    return _pinstance;
}

void MainWindow::destroy() {
    delete this;
}

MainWindow::MainWindow() {
  setWindowIcon(QIcon(":/mainwindow/m.png"));
  readSettings();

  /** Connect Other Necessary Objects */
  connect(PluginManager::instance(), SIGNAL(doneLoading()), SLOT(start()));
  connect(CommandManager::instance(), SIGNAL(quit()), SLOT(close()));

  /** Create Other Child Widgets */
  ActionManager *actMgr = ActionManager::instance(this); // Initialize ActionManager
  actMgr->createActions();
  actMgr->createMenus();
  actMgr->createToolBars();
  actMgr->createStatusBar();

  /** Create Primary Display Widgets */
  _tabWidget = new QTabWidget;
  setCentralWidget(_tabWidget);

  setCurrentProfile("test");
  qDebug() << "MainWindow created with thread:" << this->thread();
}

void MainWindow::start() {
  PluginManager::instance()->initSession(_currentProfile);
}

void MainWindow::receiveWidgets(const QList< QPair<int, QWidget*> > &widgetList) {
  // Create the layout
  QVBoxLayout *_layout = new QVBoxLayout();
  _layout->setSpacing(0);
  _layout->setContentsMargins(0, 0, 0, 0);

  QWidget *display, *input;
  bool displaySet, inputSet = false;
  for (int i = 0; i < widgetList.size(); ++i) {
    int position = widgetList.at(i).first;
    QWidget *widget = widgetList.at(i).second;
    
    // Differentiate between the types
    if (ISSET(position, DL_DISPLAY) && !displaySet) {
      // Primary Display Widget
      display = widget;
      displaySet = true;
      _layout->addWidget(widget);

    } else if (ISSET(position, DL_INPUT) && !inputSet) {
      // Primary Input Widget
      input = widget;
      inputSet = true;
      _layout->insertWidget(-1, widget); // insert to the bottom

    } else {
      // Display the widget if it floats (or is unsupported)
      QDockWidget *dockWidget = new QDockWidget;
      dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
      dockWidget->setFeatures(QDockWidget::DockWidgetMovable |
			      QDockWidget::DockWidgetFloatable |
			      QDockWidget::DockWidgetClosable);
      dockWidget->setWidget(widget);
      dockWidget->setFloating(false);
      addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
      _dockWidgets.insert("test", dockWidget);
      //widget->show();
      
    }

  }

  // Add the widgets
  _tabWidget->setLayout(_layout);

  // TODO: add tabs?
//   QWidget *widget = new QWidget();
//   widget->setLayout(_layout);
//   _tabWidget->addTab(widget, _currentProfile);

  // Connect the signals/slots
  if (displaySet && inputSet) {
    display->setFocusProxy(input);
    input->setFocus();
  }

  // Display Main Window
  show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  qDebug() << "MainWindow received closeEvent";
  PluginManager::instance()->stopSession(_currentProfile);
  writeSettings();
  if (maybeSave()) {
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::readSettings()
{
  resize(QSize(640, 480));
  move(QPoint(200, 200));
}

void MainWindow::writeSettings()
{
  ConfigManager::instance()->writeApplicationSettings();
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

MainWindow::~MainWindow() {
  PluginManager::instance()->destroy();
  ConfigManager::instance()->destroy();
  CommandManager::instance()->destroy();
  qDebug("MainWindow Destroyed");
}

void MainWindow::changeConfiguration() {
  PluginManager::instance()->configure();
}

