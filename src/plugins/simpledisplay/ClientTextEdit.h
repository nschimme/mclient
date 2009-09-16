#ifndef CLIENTTEXTEDIT_H
#define CLIENTTEXTEDIT_H

#include <QTextEdit>

class QTextDocument;

class ClientTextEdit : public QTextEdit {
    Q_OBJECT

    public:
        ClientTextEdit(QWidget* parent=0);
        ~ClientTextEdit();
    
    public slots:
        void displayText(const QString& str);
        void moveCursor(const int& diff);
        void scrollBarReleased();
	void splitterResized();
	void changeFont();

    protected:
        QTextCursor _cursor;
        QTextCharFormat _format, _defaultFormat;

 private:
	QColor _blackColor, _redColor, _greenColor, _yellowColor, _blueColor, _magentaColor;
	QColor _cyanColor, _grayColor, _darkGrayColor, _brightRedColor, _brightGreenColor;
	QColor _brightYellowColor, _brightBlueColor, _brightMagentaColor, _brightCyanColor;
	QColor _whiteColor, _foregroundColor, _backgroundColor;
	QFont _serverOutputFont, _inputLineFont;

	int previous;

	void setDefaultFormat(QTextCharFormat& format);
	void updateFormat(QTextCharFormat& format, int ansiCode);
	void updateFormatBoldColor(QTextCharFormat& format);

 signals:
	void resizeSplitter(QWidget*); // unused
};

#endif /* CLIENTTEXTEDIT_H */
