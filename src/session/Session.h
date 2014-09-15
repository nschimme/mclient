#ifndef SESSION_H
#define SESSION_H

#include <QSplitter>
#include <QDockWidget>

class MainWindow;
class WindowActionManager;

class SocketReader;
class TelnetParser;
class MumeXML;

class InputWidget;
class DisplayWebKitWidget;

class Session : public QSplitter {
    Q_OBJECT

public:
    Session(MainWindow *window = 0);
    ~Session();

    const QString name() const;

private:
    void wireSocketSignals();
    void wireTelnetSignals();
    void wireXMLSignals();

    MainWindow *_window;
    WindowActionManager *_actMgr;

    QHash<QString, QDockWidget*> _dockWidgets;
    DisplayWebKitWidget *_display;
    InputWidget *_input;

    SocketReader *_socket;
    TelnetParser *_telnet;
    MumeXML *_xml;
};

#endif /* SESSION_H */
