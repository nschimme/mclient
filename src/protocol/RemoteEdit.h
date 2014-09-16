#ifndef REMOTEEDIT_H
#define REMOTEEDIT_H

#include <QObject>

class QTimer;

class RemoteEdit : public QObject {
    Q_OBJECT
    
public:
    RemoteEdit(QObject *parent = 0);
    ~RemoteEdit();

public slots:
    void socketConnected();
    void setViewTitle(const QString&);
    void setViewBody(const QString&);
    void setEditBody(const QString&);
    void setEditTitle(const QString&);
    void setEditKey(const int);
    void cancel(const int);
    void save(const QByteArray &, const int);

signals:
    void sendToSocket(const QByteArray &);

private:
    bool _useInternalEditor;
    int _key;
    QByteArray _title;
    QByteArray _body;

    enum EditState {NORMAL, RECEIVED_KEY, RECEIVED_TITLE};
    EditState _state;
};


#endif /* REMOTEEDIT_H */
