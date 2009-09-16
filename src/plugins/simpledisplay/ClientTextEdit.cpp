#include "ClientTextEdit.h"

#include <QDebug>
#include <QScrollBar>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextCharFormat>
#include <QBrush>

#include <QFontDialog>

ClientTextEdit::ClientTextEdit(QWidget* parent) : QTextEdit(parent) {
    setReadOnly(true);
    setOverwriteMode(true);
    setUndoRedoEnabled(false);
    setDocumentTitle("mClient");
    setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
    setTabChangesFocus(false);

    //_doc->setMaximumBlockCount(Config().scrollbackSize); // max number of lines?
    document()->setUndoRedoEnabled(false);
    QTextFrame* frame = document()->rootFrame();
    _cursor = frame->firstCursorPosition();

    // Default Colors
    _foregroundColor = Qt::lightGray;
    _backgroundColor = Qt::black;
    _blackColor = Qt::darkGray;
    _redColor = Qt::darkRed;
    _greenColor = Qt::darkGreen;
    _yellowColor = Qt::darkYellow;
    _blueColor = Qt::darkBlue;
    _magentaColor = Qt::darkMagenta;
    _cyanColor = Qt::darkCyan;
    _grayColor = Qt::lightGray;
    _darkGrayColor = Qt::gray;
    _brightRedColor = Qt::red;
    _brightGreenColor = Qt::green;
    _brightYellowColor = Qt::yellow;
    _brightBlueColor = Qt::blue;
    _brightMagentaColor = Qt::magenta;
    _brightCyanColor = Qt::cyan;
    _whiteColor = Qt::white;
    // Default Fonts
    _serverOutputFont = QFont("Monospace", 10);
    _inputLineFont = QFont("Monospace", 10); //QApplication::font();
    _serverOutputFont.setStyleHint(QFont::TypeWriter, QFont::PreferAntialias);

    QTextFrameFormat frameFormat = frame->frameFormat();
    frameFormat.setBackground(_backgroundColor);
    frameFormat.setForeground(_foregroundColor);
    frame->setFrameFormat(frameFormat);

    _format = _cursor.charFormat();
    setDefaultFormat(_format);
    _defaultFormat = _format;
    _cursor.setCharFormat(_format);

    QFontMetrics fm(_serverOutputFont);
    setTabStopWidth(fm.width(" ") * 8); // A tab is 8 spaces wide
    QScrollBar* scrollbar = verticalScrollBar();
    scrollbar->setSingleStep(fm.leading()+fm.height());

    connect(scrollbar, SIGNAL(sliderReleased()), 
                this, SLOT(scrollBarReleased()));

    previous = 0;
}


ClientTextEdit::~ClientTextEdit() {
}

void ClientTextEdit::setDefaultFormat(QTextCharFormat& format) {
  format.setFont(_serverOutputFont);
  format.setBackground(_backgroundColor);
  format.setForeground(_foregroundColor);
}


void ClientTextEdit::displayText(const QString& str) {
  // ANSI codes are formatted as the following:
  // escape + [ + n1 (+ n2) + m
  QRegExp ansiRx("\\0033\\[((?:\\d+;)*\\d+)m");
  QRegExp subBlockRx("\\0010");
  QStringList blocks = str.split(ansiRx);
  QStringList ansi, subAnsi, subBlock;
  int i = 0, j, k;

  ansi << "";
  while ((i = ansiRx.indexIn(str, i)) != -1) {
    ansi << ansiRx.cap(1);
    i += ansiRx.matchedLength();
  }

  for (i = 0; i < blocks.count(); i++) {
    // split several semicoloned ansi codes into individual codes
    subAnsi = ansi[i].split(";"); 
    QStringListIterator ansiIterator(subAnsi);
    while (ansiIterator.hasNext() && i != 0)
      updateFormat(_format, ansiIterator.next().toInt());

    // split the text into sub-blocks
    blocks[i].replace((char)20, " "); // replace hex-spaces with normal spaces

    if ((k = blocks[i].indexOf(subBlockRx)) != -1) {
      j = 0;
      do {
        //qDebug("j%d k%d %s", j, k, blocks[i].toAscii().data());
        if (blocks[i].length() > 2) { // this is for the "You begin to search.." etc lines
          _cursor.insertText(blocks[i].mid(j, k), _format);
          moveCursor(-1);
        } else {                     // HACK because the twiddler has a backspace following the twiddler character
          moveCursor(-1);
          _cursor.insertText(blocks[i].mid(j, k), _format);
        }
        j = k + 1;
      } while ((k = blocks[i].indexOf(subBlockRx, j)) != -1);
      _cursor.insertText(blocks[i].mid(j), _format);
    } else _cursor.insertText(blocks[i], _format);

    /*
    for (j = 0; (j = subBlockRx.indexIn(blocks[i], j)) != -1; j += subBlockRx.matchedLength()) {
    qDebug("%s", subBlock.join(",").toAscii().data());
      if (subBlockRx.cap(1).contains(QRegExp("\\0010"))) {
        _cursor.insertText(blockIterator.next(), _format);
        moveCursor(-1);
      }
      else {
        _cursor.insertText(blockIterator.next(), _format);
      }
    }
    */
  }
  setTextCursor(_cursor);
  ensureCursorVisible();

}


void ClientTextEdit::moveCursor(const int& diff) {
  /*
  int col = _cursor.columnNumber();
  int pos = _cursor.position();
  */
  QTextCursor::MoveOperation direction;
  
  if (diff > 0) {
    direction = QTextCursor::Right;
    
  } else if(diff < 0 && _cursor.columnNumber() != 0) {
    direction = QTextCursor::Left;
    
  } else return;
  
  _cursor.movePosition(direction, QTextCursor::KeepAnchor, abs(diff));
}


void ClientTextEdit::scrollBarReleased() {
//    if(action == QAbstractSlider::SliderReleased) {
        QScrollBar* sb = verticalScrollBar();
        int val = sb->value()+sb->pageStep();
        int singleStep = sb->singleStep();
        qDebug() << "* singleStep is:" << singleStep;
        //int docSize = sb->maximum() - sb->minimum() + sb->pageStep();
        if(val%singleStep != 0) {
            qDebug() << "* val is:" << val;
            // here's what's left over
            int newVal = val/singleStep;
            int pixels = newVal*singleStep;
            qDebug() << "* newVal is:" << newVal;
            if(val-pixels < singleStep/2) {
                val = pixels;
            } else {
                val = pixels + singleStep;
            }
        }
        sb->setSliderPosition(val-sb->pageStep());
        qDebug() << "* set slider position to" << val;
//    }
}

void ClientTextEdit::updateFormat(QTextCharFormat& format, int ansiCode)
{
  /*
  if (ansiCode != previous)
    qDebug("new code: %d", ansiCode);
  else
    qDebug("repeat");
  */
  previous = ansiCode;
  QBrush tempBrush;
  switch (ansiCode) {
    case 0:
      // turn ANSI off (i.e. return to normal defaults)
      format = _defaultFormat;
      //format.clearBackground();
      //format.clearForeground();
      break;
    case 1:
      // bold
      format.setFontWeight(QFont::Bold);
      updateFormatBoldColor(format);
      break;
    case 4:
      // underline
      format.setFontUnderline(true);
      break;
    case 5:
      // blink
      // TODO
      break;
    case 7:
      // inverse
      tempBrush = format.background();
      format.setBackground(format.foreground());
      format.setForeground(tempBrush);
      break;
    case 8:
      // strike-through
      format.setFontStrikeOut(true);
      break;
    case 30:
      // black foreground
      format.setForeground(_blackColor);
      break;
    case 31:
      // red foreground
      format.setForeground(_redColor);
      break;
    case 32:
      // green foreground
      format.setForeground(_greenColor);
      break;
    case 33:
      // yellow foreground
      format.setForeground(_yellowColor);
      break;
    case 34:
      // blue foreground
      format.setForeground(_blueColor);
      break;
    case 35:
      // magenta foreground
      format.setForeground(_magentaColor);
      break;
    case 36:
      // cyan foreground
      format.setForeground(_cyanColor);
      break;
    case 37:
      // gray foreground
      format.setForeground(_grayColor);
      break;
    case 40:
      // black background
      format.setBackground(_blackColor);
      break;
    case 41:
      // red background
      format.setBackground(_redColor);
      break;
    case 42:
      // green background
      format.setBackground(_greenColor);
      break;
    case 43:
      // yellow background
      format.setBackground(_yellowColor);
      break;
    case 44:
      // blue background
      format.setBackground(_blueColor);
      break;
    case 45:
      // magenta background
      format.setBackground(_magentaColor);
      break;
    case 46:
      // cyan background
      format.setBackground(_cyanColor);
      break;
    case 47:
      // gray background
      format.setBackground(_grayColor);
      break;
    case 90:
      // high-black foreground
      format.setForeground(_darkGrayColor);
      break;
    case 91:
      // high-red foreground
      format.setForeground(_brightRedColor);
      break;
    case 92:
      // high-green foreground
      format.setForeground(_brightGreenColor);
      break;
    case 93:
      // high-yellow foreground
      format.setForeground(_brightYellowColor);
      break;
    case 94:
      // high-blue foreground
      format.setForeground(_brightBlueColor);
      break;
    case 95:
      // high-magenta foreground
      format.setForeground(_brightMagentaColor);
      break;
    case 96:
      // high-cyan foreground
      format.setForeground(_brightCyanColor);
      break;
    case 97:
      // high-white foreground
      format.setForeground(_whiteColor);
      break;
    case 100:
      // high-black background
      format.setBackground(_darkGrayColor);
      break;
    case 101:
      // high-red background
      format.setBackground(_brightRedColor);
      break;
    case 102:
      // high-green background
      format.setBackground(_brightGreenColor);
      break;
    case 103:
      // high-yellow background
      format.setBackground(_brightYellowColor);
      break;
    case 104:
      // high-blue background
      format.setBackground(_brightBlueColor);
      break;
    case 105:
      // high-magenta background
      format.setBackground(_brightMagentaColor);
      break;
    case 106:
      // high-cyan background
      format.setBackground(_brightCyanColor);
      break;
    case 107:
      // high-white background
      format.setBackground(_whiteColor);
      break;
    default:
      qDebug("Unknown!");
      format.setBackground(Qt::gray);
  };
}

void ClientTextEdit::updateFormatBoldColor(QTextCharFormat& format) {
  if (format.foreground() == _blackColor)
    format.setForeground(_darkGrayColor);
  else if (format.foreground() == _redColor)
    format.setForeground(_brightRedColor);
  else if (format.foreground() == _greenColor)
    format.setForeground(_brightGreenColor);
  else if (format.foreground() == _yellowColor)
    format.setForeground(_brightYellowColor);
  else if (format.foreground() == _blueColor)
    format.setForeground(_brightBlueColor);
  else if (format.foreground() == _magentaColor)
    format.setForeground(_brightMagentaColor);
  else if (format.foreground() == _cyanColor)
    format.setForeground(_brightCyanColor);
  else if (format.foreground() == _grayColor)
    format.setForeground(_whiteColor);
}


void ClientTextEdit::splitterResized() {
  ensureCursorVisible();
}


void ClientTextEdit::changeFont() {
  bool ok;
  QFont font
    = QFontDialog::getFont(&ok, _serverOutputFont, this);
  if (ok) {
    _serverOutputFont = font;
    _defaultFormat.setFont(_serverOutputFont);
    _cursor.setCharFormat(_defaultFormat);
    _format = _defaultFormat;
  }
}
