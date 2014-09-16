#include "RemoteEdit.h"

#include <QEvent>
#include <QVariant>
#include <QStringList>

#include <QDebug>

#include "RemoteEditWidget.h"
#include "ViewSessionProcess.h"
#include "EditSessionProcess.h"

RemoteEdit::RemoteEdit(QObject* parent)
    : QObject(parent) {
    _state = NORMAL;
    _useInternalEditor = true;
}

RemoteEdit::~RemoteEdit() {
}


void RemoteEdit::setViewTitle(const QString& title) {
    if (_state == NORMAL) {
        _key = -1;
        _title = title.toLatin1();
        _state = RECEIVED_TITLE;
        qDebug() << "* Remote editing protocol got view title" << _title;
    }
}

void RemoteEdit::setViewBody(const QString& body) {
    if (_state == RECEIVED_TITLE) {
        _body = body.toLatin1();
        qDebug() << "* Remote editing protocol got view body" << _body;
        if (_useInternalEditor) {
            new RemoteEditWidget(0, _key, _title, _body);
        } else {
            new ViewSessionProcess(_key, _title, _body, this);
        }
    }
    else if (_state == RECEIVED_KEY)
        qWarning() << "! Remote editing protocol never got a title"
                   << "; discarding body";
    else
        qWarning() << "! Remote editing protocol never got a key"
                   << "; discarding body";

    _state = NORMAL;
}
void RemoteEdit::setEditBody(const QString& body) {
    if (_state == RECEIVED_TITLE) {
        _body = body.toLatin1();
        qDebug() << "* Remote editing protocol got edit body" << _body;
        if (_useInternalEditor) {
            RemoteEditWidget *widget = new RemoteEditWidget(0, _key, _title, _body);

            connect(widget, SIGNAL(save(const QByteArray &, const int)),
                    SLOT(save(const QByteArray &, const int)));
            connect(widget, SIGNAL(cancel(const int)),
                    SLOT(cancel(const int)));
        } else {
            EditSessionProcess *process = new EditSessionProcess(_key, _title, _body, this);

            connect(process, SIGNAL(save(const QByteArray &, const int)),
                    SLOT(save(const QByteArray &, const int)));
            connect(process, SIGNAL(cancel(const int)),
                    SLOT(cancel(const int)));
        }
    }
    else if (_state == RECEIVED_KEY)
        qWarning() << "! Remote editing protocol never got a title"
                   << "; discarding body";
    else
        qWarning() << "! Remote editing protocol never got a key"
                   << "; discarding body";

    _state = NORMAL;

}
void RemoteEdit::setEditTitle(const QString& title) {
    if (_state == RECEIVED_KEY) {
        _title = title.toLatin1();
        _state = RECEIVED_TITLE;
        qDebug() << "* Remote editing protocol got edit title" << _title;
    }
    else {
        qWarning() << "! Remote editing protocol never got a key"
                   << "; discarding title";
        _state = NORMAL;

    }
}
void RemoteEdit::setEditKey(const int key) {
    if (_state != NORMAL)
        qWarning() << "! Remote editing protocol somehow received a key " << key;

    _key = key;
    _state = RECEIVED_KEY;
    qDebug() << "* Remote editing protocol got edit key" << _key;

}

void RemoteEdit::socketConnected() {
    qDebug() << "* sent MUME remote edit identify request";
    emit sendToSocket(QByteArray("~$#EI\n"));
}



void RemoteEdit::cancel(const int key) {
    // Do the whacky MUME remote editing protocol
    const QString &keystr = QString("C%1\n").arg(key);
    const QString &buffer = QString("%1E%2\n%3")
            .arg("~$#E")
            .arg(keystr.length())
            .arg(keystr);
    qDebug() << "* Edit session" << _key << "writing to socket:" << buffer;
    emit sendToSocket(buffer.toLatin1());
}

void RemoteEdit::save(const QByteArray &body, const int key) {
    // The body contents have to be followed by a LF if they are not empty
    QString content = QString(body);
    if (!content.isEmpty() && !content.endsWith('\n'))
        content.append('\n');

    // Do the whacky MUME remote editing protocol
    const QString &keystr = QString("E%1\n").arg(_key);
    const QString &buffer = QString("%1E%2\n%3%4%5")
            .arg("~$#E")
            .arg(body.length() + keystr.length())
            .arg(keystr)
            .arg(content);

    qDebug() << "* Edit session" << _key << "writing to socket:" << body;
    emit sendToSocket(buffer.toLatin1());
}
