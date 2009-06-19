#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QLineEdit>

class SimpleLineInput;

class InputWidget : public QLineEdit {
    Q_OBJECT
    
    public:
        InputWidget(QString s, SimpleLineInput *sli, QWidget* parent=0);
        ~InputWidget();

public slots:
        void toggleEchoMode(bool); 

protected slots:
        void gotInput();

    private:
	QString _session;
	SimpleLineInput* _sli;
	bool _echoMode;

 signals:
	void sendUserInput(const QString&, bool);
};


#endif /* INPUTWIDGET_H */
