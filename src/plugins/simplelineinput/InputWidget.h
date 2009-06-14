#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QLineEdit>

class SimpleLineInput;

class InputWidget : public QLineEdit {
    Q_OBJECT
    
    public:
        InputWidget(QString s, SimpleLineInput *sli, QWidget* parent=0);
        ~InputWidget();

protected slots:
        void gotInput();

    private:
	QString _session;
	SimpleLineInput* _sli;

 signals:
	void sendUserInput(const QString&);
};


#endif /* INPUTWIDGET_H */
