#ifndef STACKEDWIDGET_H
#define STACKEDWIDGET_H

#include <QStackedWidget>

class EventHandler;
class InputWidget;
class PasswordWidget;

class StackedWidget : public QStackedWidget {
    Q_OBJECT
    
    public:
        StackedWidget(EventHandler *eh, QWidget* parent=0);
        ~StackedWidget();

public slots:
        void toggleEchoMode(bool); 

    private:
	InputWidget *_inputWidget;
	PasswordWidget *_passwordWidget;
};


#endif /* STACKEDWIDGET_H */
