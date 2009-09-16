#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QLineEdit>

class InputWidget : public QLineEdit {
    Q_OBJECT
    
    public:
        InputWidget(QWidget* parent=0);
        ~InputWidget();

public slots:
        void toggleEchoMode(bool); 

protected slots:
        void gotInput();

    private:
	bool _echoMode;

 signals:
	void sendUserInput(const QString&, bool);
};


#endif /* INPUTWIDGET_H */
