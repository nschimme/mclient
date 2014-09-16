#include <QtWidgets>

#include "RemoteEditWidget.h"

RemoteEditWidget::RemoteEditWidget(QWidget *parent, int key, const QByteArray &title,
                                   const QByteArray &body)
    : QWidget(parent), _key(key), _title(title), _body(body) {
    resize(640,480);
    setAttribute(Qt::WA_DeleteOnClose);
#ifdef Q_WS_MAC
    setAttribute(Qt::AA_DontUseNativeMenuBar, true);
#endif
    setFont(QFont("Monospace", 12));

    QVBoxLayout *l=new QVBoxLayout(this);
    l->setAlignment(Qt::AlignTop);
    l->setContentsMargins(0,0,0,0);
    l->setSpacing(0);

    QMenuBar *menuBar=new QMenuBar(this);

    QMenu *fm=menuBar->addMenu(tr("&File"));
    QAction *_save =new QAction(tr("&Save"), this);
    fm->addAction(_save);
    QAction *_quit = new QAction(tr("&Quit"), this);
    fm->addAction(_quit);

    QMenu *em=menuBar->addMenu(tr("&Edit"));
    em->addAction(new QAction(tr("Cu&t"), this));
    em->addAction(new QAction(tr("&Copy"), this));
    em->addAction(new QAction(tr("&Paste"), this));

    QMenu *vm=menuBar->addMenu(tr("&Help"));
    vm->addAction(new QAction(tr("Item 1"), this));
    vm->addAction(new QAction(tr("Item 2"), this));
    vm->addAction(new QAction(tr("Item 3"), this));

    l->addWidget(menuBar);

    _textEdit=new QTextEdit(this);
    _textEdit->setText(_body);
    _textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    l->addWidget(_textEdit);

    setWindowTitle(tr(_title));
    setFocus();

    connect(_save, SIGNAL(triggered()),
            SLOT(finishEdit()));

    connect(_quit, SIGNAL(triggered()),
            SLOT(cancelEdit()));

    show();
}

RemoteEditWidget::~RemoteEditWidget()
{
}

void RemoteEditWidget::closeEvent(QCloseEvent * event) {
    qDebug() << "MainWindow received closeEvent";
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool RemoteEditWidget::maybeSave() {
    if (contentsChanged()) {
        int ret = QMessageBox::warning(this, tr(_title),
                                       tr("You have edited the buffer.\n"
                                          "Do you want to save your changes?"),
                                       QMessageBox::Yes | QMessageBox::Default,
                                       QMessageBox::No,
                                       QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            finishEdit();
        else if (ret == QMessageBox::No) {
            cancelEdit();
        }
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

bool RemoteEditWidget::contentsChanged() {
    QString text = _textEdit->toPlainText();
    return text.compare(_body) != 0;
}

void RemoteEditWidget::cancelEdit() {
    emit cancel(_key);
    close();
}


void RemoteEditWidget::finishEdit() {
    emit save(_textEdit->toPlainText().toLatin1(), _key);
    close();
}

