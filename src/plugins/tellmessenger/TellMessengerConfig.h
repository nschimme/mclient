#ifndef SOCKETMANAGERIOCONFIG_H
#define SOCKETMANAGERIOCONFIG_H

#include <QWidget>

class QLineEdit;


class TellMessengerConfig : public QWidget {
    Q_OBJECT

    public:
        TellMessengerConfig(QWidget* parent=0);
        ~TellMessengerConfig();

    private:
        QLineEdit* _hostLineEdit;
        QLineEdit* _portLineEdit;

};


#endif /* SOCKETMANAGERIOCONFIG_H */
