#include "WebKitDisplay.h"
#include "DisplayWidget.h"

#include "PluginManager.h"
#include "PluginSession.h"
#include "MainWindow.h"
#include "MClientEvent.h"

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QVariant>
#include <QtWebKit>

Q_EXPORT_PLUGIN2(webkitdisplay, WebKitDisplay)

const QChar WebKitDisplay::greaterThanChar('>');
const QChar WebKitDisplay::lessThanChar('<');
const QString WebKitDisplay::greaterThanTemplate("&gt;");
const QString WebKitDisplay::lessThanTemplate("&lt;");

WebKitDisplay::WebKitDisplay(QWidget* parent) 
        : MClientDisplayPlugin(parent) {
    _shortName = "webkitdisplay";
    _longName = "WebKit Display";
    _description = "A display plugin using WebKit.";
//   _dependencies.insert("terrible_test_api", 1);
//    _implemented.insert("some_other_api",1);
    _receivesDataTypes << "DisplayData";
    //_deliversDataTypes << "?";
    _configurable = false;
    _configVersion = "2.0";

    // Set up the Browser
    QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, false);

    // Debugging
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    // Allowable Display Locations
    SET(_displayLocations, DL_DISPLAY);

    // ANSI
    _foreground = false;
    _background = false;
    _bold = false;
    _underline = false;
    _blink = false;
    _inverse = false;
    _strikethrough = false;
}


WebKitDisplay::~WebKitDisplay() {
    saveSettings();
}


void WebKitDisplay::customEvent(QEvent* e) {
//    qDebug() << "* bork bork bork!";
    if(!e->type() == 10001) return;
    
    MClientEvent* me;
    me = static_cast<MClientEvent*>(e);

    if(me->dataTypes().contains("DisplayData")) {
      parseDisplayData(me->payload()->toString());
    }
}

void WebKitDisplay::parseDisplayData(QString text) {

  text.replace(greaterThanChar, greaterThanTemplate);
  text.replace(lessThanChar, lessThanTemplate);
  text.replace(QString("\r\n"), QString("<br>"));
  text.replace(QString("\n"), QString("<br>"));
  text.replace(QChar('\''), QString("\\'"));
  text.replace(QChar('`'), QString("\\`"));
  text.replace(QChar('$'), QString("\\$"));

  // ANSI Removal
  QRegExp ansiRx("\\0033\\[((?:\\d+;)*\\d+)m");
  int index = ansiRx.indexIn(text);
  QStringList ansi = ansiRx.capturedTexts();
  QStringList blocks = text.split(ansiRx);

  QString output;
  // Does ANSI or a text block start?
  if (index == 0) {
    output += convertANSI(ansi.takeFirst().toInt());
  } else {
    output += blocks.takeFirst();
  }
  
  /*
  qDebug() << blocks.count() << blocks << "\n"
	   << ansi.count() << ansi;
  */
  
  // Add subsequent ANSI codes
  for (int i = 0; i < blocks.count(); i++) {
    //qDebug() << i << blocks.at(i);
    // Add current output block
    output += blocks.at(i);
    
    // Split concatenated ANSI codes
    if (index >= 0 && i < ansi.count()) {
      QStringList codes = ansi[i].split(";"); 
      for (int j = 0; j < codes.size(); j++)
	output += convertANSI(codes.at(j).toInt());
    }
  }
  
  emit dataReceived(output);
}

QString WebKitDisplay::convertANSI(int code) {
  QString format("");
  switch (code) {
  case 0:
    if (_foreground) {
      format += "</span>";
      _foreground = false;
    }
    if (_background) {
      format += "</span>";
      _background = false;
    }
    if (_bold) {
      format += "</span>";
      _bold = false;
    }
    if (_underline) {
      format += "</span>";
      _underline = false;
    }
    if (_blink) {
      format += "</span>";
      _blink = false;
    }
    if (_inverse) {
      format += ""; // TODO
      _inverse = false;
    }
    if (_strikethrough) {
      format += "</span>";
    }
    break;
  case 1: 
    if (!_bold) {
      format = "<span class=\"bold\">";
      _bold = true;
    }
    break;
  case 2:
    if (!_underline) {
      format = "<span class=\"underline\">";
      _underline = true;
    }
    break;
  case 5:
    if (!_blink) {
      format = "<span class=\"blink\">";
      _blink = true;
    }
    break;
  case 7:
    if (!_inverse) { // TODO
      format = "";
      _inverse = true;
    }
    break;
  case 8:
    if (!_strikethrough) {
      format = "<span class=\"strikethrough\">";
      _strikethrough = true;
    }
    break;
    // Foregrounds
  case 30: format = "black"; break;
  case 31: format = "red"; break;
  case 32: format = "green"; break;
  case 33: format = "yellow"; break;
  case 34: format = "blue"; break;
  case 35: format = "magenta"; break;
  case 36: format = "cyan"; break;
  case 37: format = "gray"; break;
    // High Foregrounds
  case 90: format = "darkgray"; break;
  case 91: format = "brightred"; break;
  case 92: format = "brightgreen"; break;
  case 93: format = "brightyellow"; break;
  case 94: format = "brightblue"; break;
  case 95: format = "brightmagenta"; break;
  case 96: format = "brightcyan"; break;
  case 97: format = "white"; break;
    // Backgrounds
  case 40: format = "black"; break;
  case 41: format = "red"; break;
  case 42: format = "green"; break;
  case 43: format = "yellow"; break;
  case 44: format = "blue"; break;
  case 45: format = "magenta"; break;
  case 46: format = "cyan"; break;
  case 47: format = "gray"; break;
    // High Backgrounds
  case 100: format = "darkgray"; break;
  case 101: format = "brightred"; break;
  case 102: format = "brightgreen"; break;
  case 103: format = "brightyellow"; break;
  case 104: format = "brightblue"; break;
  case 105: format = "brightmagenta"; break;
  case 106: format = "brightcyan"; break;
  case 107: format = "white"; break;
    // Other
  default:
    qDebug() << "Unknown ANSI code" << code;
    break;
  };

  // Foreground
  if ((code >= 30 && code <= 37) || (code >= 90 && code <= 97)) {
    format = QString("<span class=\"%1\">").arg(format);
    if (!_foreground)
      _foreground = true;
    else
      format += "</span>";
  }

  // Background
  if ((code >= 40 && code <= 47) || (code >= 100 && code <= 107)) {
    format = QString("<span class=\"%1-bg\">").arg(format);
    if (!_background)
      _background = true;
    else
      format += "</span>";
  }

  qDebug() << "* got ANSI:" << code << format;
  return format;
}

void WebKitDisplay::configure() {
}


const bool WebKitDisplay::loadSettings() {
 return true;
}

        
const bool WebKitDisplay::saveSettings() const {
  return true;
}


const bool WebKitDisplay::startSession(QString s) {
    initDisplay(s);
    return true;
}


const bool WebKitDisplay::stopSession(QString s) {
  if (_widget->close())
    qDebug() << "* removed WebKit DisplayWidget for session" << s;
  return true;
}


// Display plugin members
const bool WebKitDisplay::initDisplay(QString s) {
  MainWindow *mw = _pluginSession->getManager()->getMainWindow();
  _widget = new DisplayWidget(s, this, mw);
  
  return true;
}

QWidget* WebKitDisplay::getWidget(QString s) {
    return _widget;
}
