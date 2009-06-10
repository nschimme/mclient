#ifndef SOCKETMANAGERIOCONFIG_H
#define SOCKETMANAGERIOCONFIG_H

#include <QWidget>

class QLineEdit;


class SocketManagerIOConfig : public QWidget {
    Q_OBJECT

    public:
        SocketManagerIOConfig(QWidget* parent=0);
        ~SocketManagerIOConfig();

    private:
        QLineEdit* _hostLineEdit;
        QLineEdit* _portLineEdit;

};


#endif /* SOCKETMANAGERIOCONFIG_H */
