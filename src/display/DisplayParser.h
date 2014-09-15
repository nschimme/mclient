#ifndef DISPLAYPARSER_H
#define DISPLAYPARSER_H

#include <QObject>

class DisplayParser : public QObject {
    Q_OBJECT
    
public:
    DisplayParser(QObject *parent=0);
    ~DisplayParser();

    QString parseDisplayData(const QString&);

protected:
    static const QChar greaterThanChar;
    static const QChar lessThanChar;
    static const QString greaterThanTemplate;
    static const QString lessThanTemplate;

private:
    bool _foreground, _background, _bold, _underline;
    bool _blink, _inverse, _strikethrough;

    QString convertANSI(int code);

signals:
    void displayText(const QString&);

};

#endif /* DISPLAYPARSER_H */
