#include "DisplayParser.h"

#include <QChar>
#include <QString>
#include <QRegExp>
#include <QStringList>
#include <QDebug>
#include <QTextDocument>

const QChar DisplayParser::greaterThanChar('>');
const QChar DisplayParser::lessThanChar('<');
const QString DisplayParser::greaterThanTemplate("&gt;");
const QString DisplayParser::lessThanTemplate("&lt;");

DisplayParser::DisplayParser(QObject* parent) : QObject(parent) {
  // ANSI
  _foreground = false;
  _background = false;
  _bold = false;
  _underline = false;
  _blink = false;
  _inverse = false;
  _strikethrough = false;

  // Debugging Information
  qDebug() << "* DisplayParser thread:" << this->thread();
}


DisplayParser::~DisplayParser() {
}


void DisplayParser::displayData(const QString &text) {
  parseDisplayData(text);
}


void DisplayParser::userInput(const QString &text) {
  parseDisplayData(text);
}


void DisplayParser::parseDisplayData(QString text) {
  text = Qt::escape(text);

  // ANSI Removal
  QRegExp ansiRx("\\0033\\[((?:\\d+;)*\\d+)m");

  QString output;
  int old = 0, pos = 0;
  while ((pos = ansiRx.indexIn(text, old)) != -1) {    
    int diff = pos - old;
    output.append(text.midRef(old, diff));

    const QStringList &codes = ansiRx.cap(1).split(";");
    foreach (const QString &code, codes) {
      output.append(convertANSI(code.toInt()));
    }
    old += diff + ansiRx.matchedLength();
  }
  output.append(text.midRef(old));

  // Replace URLs with links
  QRegExp urlRx("(\\b(https?|ftp|file):\\/\\/[a-zA-Z0-9+&@#\\/%?=~_|!:,.;]*[a-zA-Z0-9+&@#\\/%=~_|])");
  output.replace(urlRx, "<a href=\"\\1\" target=\"_blank\">\\1</a>");

  emit displayText(output);
}

QString DisplayParser::convertANSI(int code) {
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
