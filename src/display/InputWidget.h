#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QStackedWidget>

class InputMultiWidget;
class InputPasswordWidget;

class InputWidget : public QStackedWidget {
    Q_OBJECT
    
    public:
        InputWidget(QWidget* parent=0);
        ~InputWidget();

public slots:
        void toggleEchoMode(bool); 

    private:
    InputMultiWidget *_inputWidget;
    InputPasswordWidget *_passwordWidget;
};


#endif /* INPUTWIDGET_H */
