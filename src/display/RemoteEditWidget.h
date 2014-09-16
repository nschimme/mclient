#ifndef REMOTEEDITWIDGET_H
#define REMOTEEDITWIDGET_H

#include <QWidget>
#include <QAction>
#include <QTextEdit>

class RemoteEdit;

class RemoteEditWidget : public QWidget {
    Q_OBJECT

public:
    RemoteEditWidget(QWidget *parent, const QByteArray &title, const QByteArray &body);
    RemoteEditWidget(QWidget *parent, int key, const QByteArray &title, const QByteArray &body);
    ~RemoteEditWidget();

    void closeEvent(QCloseEvent * event);

protected slots:
    void cancelEdit();
    void finishEdit();
    bool maybeSave();
    bool contentsChanged();

signals:
    void cancel(const int);
    void save(const QByteArray &, const int);

private:
    const QByteArray& _title;
    const QByteArray& _body;
    const int _key;

    QTextEdit *_textEdit;

    friend class RemoteEdit;
};

#endif // REMOTEEDITWIDGET_H
