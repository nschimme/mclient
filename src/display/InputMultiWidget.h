#ifndef INPUTMULTIWIDGET_H
#define INPUTMULTIWIDGET_H

#include <QPlainTextEdit>

class InputMultiWidget : public QPlainTextEdit {
    Q_OBJECT
    
public:
    InputMultiWidget(QWidget* parent=0);
    ~InputMultiWidget();

    QSize sizeHint() const;
    void setEchoMode(bool echoMode);

protected:
    void keyPressEvent(QKeyEvent *event);

public slots:
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
    void sendUserInput(const QString&);
    void resizeSplitter(QWidget*);
    void displayMessage(const QString &);
};


#endif /* INPUTMULTIWIDGET_H */
