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

 protected:
	SimpleLineInput* _sli;

    private:
	QString _session;
};


#endif /* INPUTWIDGET_H */
