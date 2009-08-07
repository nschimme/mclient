#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QPlainTextEdit>

class MultiLineInput;

class InputWidget : public QPlainTextEdit {
    Q_OBJECT
    
    public:
        InputWidget(QString s, MultiLineInput *mli, QWidget* parent=0);
        ~InputWidget();

	QSize sizeHint() const;

	//void customEvent(QEvent *e);

 protected:
	void keyPressEvent(QKeyEvent *event);

public slots:
        void toggleEchoMode(bool); 

    private:
	QString _session;
	MultiLineInput* _mli;
	bool _echoMode;

        void gotInput();
	void detectedLineChange();
	void wordHistory(int);

 signals:
	void sendUserInput(const QString&, bool);
	void resizeSplitter(QWidget*);
};


#endif /* INPUTWIDGET_H */
