#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QStackedWidget>
#include <QLineEdit>

class InputMultiWidget;
class Session;

class InputWidget : public QStackedWidget {
    Q_OBJECT
    
public:
    InputWidget(QWidget* parent=0);
    ~InputWidget();

public slots:
    void toggleEchoMode(bool);
    void gotPasswordInput();
    void gotMultiLineInput(QString);
    void relayMessage(const QString &);

signals:
    void sendUserInput(const QByteArray&);
    void displayMessage(const QString &);

private:
    bool _localEcho;
    InputMultiWidget *_inputWidget;
    QLineEdit *_passwordWidget;

    friend class Session;
};


#endif /* INPUTWIDGET_H */
