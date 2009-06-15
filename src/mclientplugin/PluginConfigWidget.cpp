#include "PluginConfigWidget.h"

#include "MClientPluginInterface.h"

#include <QDebug>
#include <QPluginLoader>
#include <QPushButton>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QTextEdit>
#include <QTreeView>
#include <QVBoxLayout>


PluginConfigWidget::PluginConfigWidget(QHash<QString, QPluginLoader*> plugins,
        QWidget* parent) : QWidget(parent) {

    _plugins = plugins;
    //qDebug() << "* plugins for PluginConfigWidget:" << _plugins;

    QWidget* window = new QWidget();
    QStandardItemModel* model = new QStandardItemModel(window);
    QStandardItem* parentItem = model->invisibleRootItem();
        
    QStringList labels;
    labels << "Plugin" << "Description";
    model->setHorizontalHeaderLabels(labels);
    
    QPluginLoader* pl;
    foreach(pl, _plugins) {
        MClientPluginInterface* ip = 
                qobject_cast<MClientPluginInterface*>(pl->instance());

        if(!ip) continue;
        QList<QStandardItem*> itemList;
        QStandardItem* first = new QStandardItem(ip->longName());
        first->setData(ip->shortName(), Qt::UserRole);
        itemList.append(first);
        itemList.append(new QStandardItem(ip->description()));
        QStandardItem* item;
        foreach(item, itemList) {
            item->setEditable(false);
        }
        parentItem->appendRow(itemList);

    }
    
    _tree = new QTreeView(this);
    _tree->setModel(model);
    _tree->setColumnWidth(0,200);
    _tree->setRootIsDecorated(false);
    _tree->setSelectionMode(QAbstractItemView::SingleSelection);
    _tree->setAlternatingRowColors(true);
    _tree->setWordWrap(true);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addWidget(_tree);

    _descBox = new QTextEdit();
    //_descBox->setEditable(false);
    vlayout->addWidget(_descBox);

    _configButton = new QPushButton(this);
    _configButton->setText("Configure...");
    vlayout->addWidget(_configButton);

    this->setLayout(vlayout);
    this->setMinimumSize(600,300);

    // Needed to ensure that when the selection changes, we hear it
    connect(_tree->selectionModel(), 
            SIGNAL(selectionChanged(const QItemSelection&, 
                    const QItemSelection&)), 
            this, 
            SLOT(updateSelection(const QItemSelection&, 
                    const QItemSelection&)));

    connect(_configButton, SIGNAL(pressed()), this, SLOT(on_configure()));
}


PluginConfigWidget::~PluginConfigWidget() {
}


void PluginConfigWidget::updateSelection(const QItemSelection &selected,
					 const QItemSelection &) {

    QModelIndex index;
    QModelIndexList items = selected.indexes();
    
    // Check whether the item represented by index is configurable, and if
    // so, enable the Configure..." button.
    foreach(index, items) {
        if(index.column() == 0) {
            QString sn = index.data(Qt::UserRole).toString();
            QHash<QString, QPluginLoader*>::iterator it = _plugins.find(sn);
            if(it != _plugins.end()) {
                MClientPluginInterface* pi = 
                    qobject_cast<MClientPluginInterface*>(
                            it.value()->instance());
                if(!pi) {
                    qDebug() << "couldn't cast!";
                    return;
                }
                _descBox->setText(pi->description());
                if(pi->configurable()) {
                    //qDebug() << sn << "is configurable";
                    _configButton->setEnabled(true);
                } else {
                    //qDebug() << sn << "is NOT configurable";
                    _configButton->setDisabled(true);
                }
            }
        }
    }
}


void PluginConfigWidget::on_configure() {
    // The Configure... button has been pressed, so we want to get the plugin
    // at the current index and configure() it.

    qDebug() << "Configuring!";
    //qDebug() << _tree->currentIndex().row();
    QString sn = 
        _tree->model()->index(_tree->currentIndex().row(),0)
        .data(Qt::UserRole).toString();
    QHash<QString, QPluginLoader*>::iterator it = _plugins.find(sn);
    if(it != _plugins.end()) {
        MClientPluginInterface* pi = 
            qobject_cast<MClientPluginInterface*>(it.value()->instance());
        if(!pi) {
            qDebug() << "couldn't cast!";
            return;
        }

        pi->configure();
    }
}
