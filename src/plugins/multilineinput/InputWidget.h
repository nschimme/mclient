#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QPlainTextEdit>

class InputWidget : public QPlainTextEdit {
    Q_OBJECT
    
    public:
        InputWidget(QWidget* parent=0);
        ~InputWidget();

	QSize sizeHint() const;

	//void customEvent(QEvent *e);

 protected:
	void keyPressEvent(QKeyEvent *event);

public slots:
        void toggleEchoMode(bool); 
        void showCommandHistory(); 
	void addTabHistory(const QStringList &);

    private:
	bool _echoMode;

        void gotInput();
	void detectedLineChange();
	void wordHistory(int);
	void keypadMovement(int);

	QMutableStringListIterator *_lineIterator;
	bool _newInput;
	QStringList _lineHistory, _wordHistory;

	void addLineHistory(const QString &);
	void forwardHistory();
	void backwardHistory();

	void tabWord();

	enum TabState {NORMAL, TABBING};
	TabState _tabState;
	QString _tabFragment;
	QMutableStringListIterator *_tabIterator;
	void addTabHistory(const QString &);

 signals:
	void sendUserInput(const QString&, bool);
	void resizeSplitter(QWidget*);
	void displayMessage(const QString &);
};


#endif /* INPUTWIDGET_H */
