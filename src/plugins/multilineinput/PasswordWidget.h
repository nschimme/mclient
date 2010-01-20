#ifndef PASSWORDWIDGET_H
#define PASSWORDWIDGET_H

#include <QLineEdit>

class PasswordWidget : public QLineEdit {
    Q_OBJECT
    
    public:
        PasswordWidget(QWidget* parent=0);
        ~PasswordWidget();

protected slots:
        void gotInput();

 signals:
	void sendUserInput(const QString&, bool);
};


#endif /* PASSWORDWIDGET_H */
