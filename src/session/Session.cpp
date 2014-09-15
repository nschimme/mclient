#include "Session.h"

#include "MainWindow.h"
#include "WindowActionManager.h"

#include "SocketReader.h"
#include "TelnetParser.h"
#include "MumeXML.h"

#include "InputWidget.h"
#include "DisplayWebKitWidget.h"

#include <QString>
#include <QMenu>
#include <QMenuBar>

Session::Session(MainWindow *window)
    : QSplitter(window), _window(window), _actMgr(window->_actMgr)
{
    setOrientation(Qt::Vertical);

    // Create children plugins and object
    _socket = new SocketReader(this);
    _telnet = new TelnetParser(this);
    _xml = new MumeXML(this);

    // Add the primary widgets to the smart splitter
    _display = new DisplayWebKitWidget(this);
    _input = new InputWidget(this);

    addWidget(_display);
    setCollapsible(indexOf(_display), false);

    addWidget(_input);
    setCollapsible(indexOf(_input), false);

    /*
      // Display the widget if it floats (or is unsupported)
      QDockWidget *dockWidget = new QDockWidget(this);
      dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
      dockWidget->setFeatures(QDockWidget::DockWidgetMovable
                  | QDockWidget::DockWidgetFloatable
                  );
      dockWidget->setWidget(widgetList[i].second);
      dockWidget->setFloating(false);
      addDockWidget(Qt::RightDockWidgetArea, dockWidget);
      _dockWidgets.insert("test", dockWidget);
      //widgetList.at(i).second->show();
    */

    // Display Main Window
    qDebug() << "* Displaying session" << name();

    // Connect the signals/slots
    _display->setFocusProxy(_input);
    _input->setFocus();

    connect(_actMgr->copyAct, SIGNAL(triggered()),
            _display, SLOT(copy()));
    connect(_actMgr->pasteAct, SIGNAL(triggered()),
            _input, SLOT(paste()));
    connect(_display, SIGNAL(copyAvailable(bool)),
            _actMgr->cutAct, SLOT(setEnabled(bool)));
    connect(_display, SIGNAL(copyAvailable(bool)),
            _actMgr->copyAct, SLOT(setEnabled(bool)));
    connect(_actMgr->cutAct, SIGNAL(triggered()),
            _display, SLOT(cut()));

    wireSocketSignals();
    wireTelnetSignals();
    wireXMLSignals();

    // Input
    connect(_input, SIGNAL(sendUserInput(const QByteArray&)),
            _telnet, SLOT(unIACData(const QByteArray&)));
    connect(_input, SIGNAL(displayMessage(const QString &)),
            _display, SLOT(appendText(const QString &)));
    /*
    connect(eh, SIGNAL(showCommandHistory()),
        _inputWidget, SLOT(showCommandHistory()));
    connect(eh, SIGNAL(addTabHistory(const QStringList &)),
        _inputWidget, SLOT(addTabHistory(const QStringList &)));
    */
}

Session::~Session() {
    _socket->deleteLater();
    _telnet->deleteLater();
    _xml->deleteLater();
}

void Session::wireSocketSignals() {
    QMenu *fileMenu = _actMgr->fileMenu;

    QAction *connectAct = new QAction(tr("&Connect"), 0);
    connectAct->setStatusTip(tr("Connect to the remote host"));
    connect(connectAct, SIGNAL(triggered()),
            _socket, SLOT(connectToHost()) );

    QAction *disconnectAct = new QAction(tr("&Disconnect"), 0);
    disconnectAct->setStatusTip(tr("Disconnect from the host"));
    connect(disconnectAct, SIGNAL(triggered()),
            _socket, SLOT(closeSocket()) );

    fileMenu->addAction(connectAct);
    fileMenu->addAction(disconnectAct);

    connect(_socket, SIGNAL(socketReadData(const QByteArray &)),
            _telnet, SLOT(readData(const QByteArray &)));
    connect(_socket, SIGNAL(socketOpened()),
            _telnet, SLOT(socketConnected()));
    connect(_socket, SIGNAL(socketOpened()),
            _xml, SLOT(socketConnected()));
    connect(_socket, SIGNAL(displayMessage(const QString &)),
            _display, SLOT(appendText(const QString &)));
}

void Session::wireTelnetSignals() {
    /*
    connect(_telnet, SIGNAL(displayData(const QString &, bool)),
            _display, SLOT(appendText(const QString &)));
    */

    connect(_telnet, SIGNAL(displayData(const QString&, bool)),
            _xml, SLOT(parse(const QString&, bool)));

    connect(_telnet, SIGNAL(socketWrite(const QByteArray &)),
            _socket, SLOT(sendToSocket(const QByteArray &)));

    connect(_telnet, SIGNAL(echoModeChanged(bool)),
            _input, SLOT(toggleEchoMode(bool)));

    /*
     void displayData(const QString &data, bool goAhead);
    void socketWrite(const QByteArray &data);
    void echoModeChanged(bool);
    */
}

void Session::wireXMLSignals() {
    connect(_xml, SIGNAL(sendMpiXmlRequest(const QByteArray&)),
            _socket, SLOT(sendToSocket(QByteArray)));

    connect(_xml, SIGNAL(xmlNone(const QString &)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlDynamicDescription(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlRoomName(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlRoomDescription(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlRoomExits(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlPrompt(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlTerrain(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlMagic(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlTell(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlNarrate(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlSong(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlPray(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlShout(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlYell(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlEmote(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlSay(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlDamage(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlHit(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlWeather(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlAvoidDamage(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlMovement(const QString&)), _display, SLOT(appendText(const QString &)));

    /*
    connect(_xml, SIGNAL(xmlViewTitle(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlViewBody(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlEditBody(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlEditTitle(const QString&)), _display, SLOT(appendText(const QString &)));
    connect(_xml, SIGNAL(xmlEditKey(const int)), _display, SLOT(appendText(const int)));
    */
}

const QString Session::name() const {
    return "MUME";
}
