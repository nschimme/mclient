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
        void showCommandHistory(); 

    private:
	QString _session;
	MultiLineInput* _mli;
	bool _echoMode;

        void gotInput();
	void detectedLineChange();
	void wordHistory(int);

	QMutableStringListIterator *_iterator;
	bool _newInput;
	QStringList _wordHistory;

	void addHistory(const QString);
	void forwardHistory();
	void backwardHistory();

 signals:
	void sendUserInput(const QString&, bool);
	void resizeSplitter(QWidget*);
	void displayMessage(const QString &);
};


#endif /* INPUTWIDGET_H */
