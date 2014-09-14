#ifndef INPUTPASSWORDWIDGET_H
#define INPUTPASSWORDWIDGET_H

#include <QLineEdit>

class InputPasswordWidget : public QLineEdit {
    Q_OBJECT
    
    public:
        InputPasswordWidget(QWidget* parent=0);
        ~InputPasswordWidget();

protected slots:
        void gotInput();

 signals:
	void sendUserInput(const QString&, bool);
};


#endif /* INPUTPASSWORDWIDGET_H */
