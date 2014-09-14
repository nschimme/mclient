#ifndef DISPLAYWEBKITWIDGET_H
#define DISPLAYWEBKITWIDGET_H

#include <QtWebKitWidgets>
#include <QWidget>
#include <QWebView>

class DisplayWebKitWidget : public QWebView {
    Q_OBJECT
    
public:
    DisplayWebKitWidget(QWidget* parent=0);
    ~DisplayWebKitWidget();

public slots:
    void appendText(const QString&);
    void scrollToBottom();
    void loadFrame(QWebFrame *frame);

protected slots:
    void finishLoading(bool);

protected:
    QWebView* _view;
    void paintEvent(QPaintEvent *ev);

private:
    int _currentSection, _maxSections;
    int _currentCharacterCount, _maxCharacterCount;

    bool _scrollToBottom;
};


#endif /* DISPLAYWEBKITWIDGET_H */
