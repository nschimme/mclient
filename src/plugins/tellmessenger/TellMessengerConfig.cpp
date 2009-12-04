#include "TellMessengerConfig.h"

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>

TellMessengerConfig::TellMessengerConfig(QWidget* parent) 
    : QWidget(parent) {
    
    _hostLineEdit = new QLineEdit(this);
    _portLineEdit = new QLineEdit(this);

    QLabel* hostLabel = new QLabel("Host:",this);
//    hostLabel->setText("Host:");
    QLabel* portLabel = new QLabel(this);
    portLabel->setText("Port:");

    QHBoxLayout* hb1 = new QHBoxLayout();
    QGroupBox* gb1 = new QGroupBox(this);
    gb1->setLayout(hb1);
    hb1->addWidget(hostLabel);
    hb1->addWidget(_hostLineEdit);

    QHBoxLayout* hb2 = new QHBoxLayout();
    QGroupBox* gb2 = new QGroupBox(this);
    gb2->setLayout(hb2);
    hb2->addWidget(portLabel);
    hb2->addWidget(_portLineEdit);
    
    QPushButton* acceptButton = new QPushButton("Accept", this);
    QPushButton* cancelButton = new QPushButton("Cancel", this);
    QHBoxLayout* hb3 = new QHBoxLayout();
    QGroupBox* gb3 = new QGroupBox(this);
    gb3->setLayout(hb3);
    hb3->addWidget(acceptButton);
    hb3->addWidget(cancelButton);

    
    QVBoxLayout* vb1 = new QVBoxLayout();
    vb1->addWidget(gb1);
    vb1->addWidget(gb2);
    vb1->addWidget(gb3);

    this->setMinimumSize(400,400);
    this->setLayout(vb1);

}


TellMessengerConfig::~TellMessengerConfig() {

}
