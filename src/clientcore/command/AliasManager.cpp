#include "AliasManager.h"

#include <QDebug>
#include <QVariant>
#include <QAbstractItemModel>
#include <QColor>

class AliasItem {
public:
  AliasItem(const QList<QVariant> &data, bool group=false,
	    AliasItem *parent=0) {
    _parentItem = parent;
    _itemData = data;
    _isGroup = group;
  }
  
  ~AliasItem() {
    qDeleteAll(_childItems); 
  }
  
  void appendChild(AliasItem *item) {
    _childItems.append(item);
  }
  
  AliasItem *child(int row) {
    return _childItems.value(row);
  }
  
  int childCount() const {
    return _childItems.count();
  }
  
  int columnCount() const {
    return _itemData.count();
  }
  
  QVariant data(int column) const {
    return _itemData.value(column);
  }
  
  AliasItem *parent() {
    return _parentItem;
  }

  int row() const {
    if (_parentItem)
      return _parentItem->_childItems.indexOf(const_cast<AliasItem*>(this));
    return 0;
  }

  bool isGroup() const {
    return _isGroup;
  }
  
private:
  QList<AliasItem*> _childItems;
  QList<QVariant> _itemData;
  AliasItem *_parentItem;
  bool _isGroup;
};


class AliasModel : public QAbstractItemModel {
public:
  AliasModel (AliasManager *manager, QObject *parent=0)
    : QAbstractItemModel(parent), _mgr(manager) {
    QList<QVariant> rootData;
    rootData << "Name";
    _rootItem = new AliasItem(rootData);
    setupModelData(_rootItem);
  }
  
  ~AliasModel() {
    delete _rootItem;
  }
  
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const {
    if (parent.isValid())
      return static_cast<AliasItem*>(parent.internalPointer())->columnCount();
    else
      return _rootItem->columnCount();
  }
  
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const {
    AliasItem *parentItem;
    if (parent.column() > 0)
      return 0;   
    if (!parent.isValid())
      parentItem = _rootItem;
    else
      parentItem = static_cast<AliasItem*>(parent.internalPointer());
    return parentItem->childCount();
  }
  
  virtual QVariant headerData(int section, Qt::Orientation orientation,
			      int role = Qt::DisplayRole) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return _rootItem->data(section);    
    return QVariant();
  }

  QModelIndex index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
      return QModelIndex();
    AliasItem *parentItem;
    if (!parent.isValid())
      parentItem = _rootItem;
    else
      parentItem = static_cast<AliasItem*>(parent.internalPointer());
    AliasItem *childItem = parentItem->child(row);
    if (childItem)
      return createIndex(row, column, childItem);
    else
      return QModelIndex();
  }
  
  virtual QVariant data(const QModelIndex &index,
			int role = Qt::DisplayRole) const {
    if (!index.isValid())
      return QVariant();
    AliasItem *item = static_cast<AliasItem*>(index.internalPointer());
    switch (role) {
    case Qt::DisplayRole:
      return item->data(index.column());
    case Qt::DecorationRole:
      return qVariantValue<QColor>(QColor(Qt::red));
    case Qt::CheckStateRole: 
      return QVariant(Qt::Checked);
    case Qt::ToolTipRole:
    default:
      return QVariant();
    };
  }
  
  Qt::ItemFlags flags(const QModelIndex &index) const {
    if (!index.isValid())
      return 0;
    AliasItem *item = static_cast<AliasItem*>(index.internalPointer());
    if (item->isGroup())
      return Qt::ItemIsEnabled;
    else
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
  }
  
  QModelIndex parent(const QModelIndex &index) const {
    if (!index.isValid())
      return QModelIndex();
    AliasItem *childItem = static_cast<AliasItem*>(index.internalPointer());
    AliasItem *parentItem = childItem->parent();
    if (parentItem == _rootItem)
      return QModelIndex();
    return createIndex(parentItem->row(), 0, parentItem);
  }
  
  /* TODO
  void rowChanged(int row) {
    emit dataChanged(index(row, 0), index(row, 3));
  }
  
  void insertRows(int position, int rows, const QModelIndex &parent) {
    AliasItem *parentItem = getItem(parent);
    bool success;
    beginInsertRows(parent, position, position + rows - 1);
    success =
      parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();
    return success;
  }

  bool removeRows(int position, int rows, const QModelIndex &parent) {
    AliasItem *parentItem = getItem(parent);
    bool success = true;
    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();
    return success;
  }
  */

  private:
  AliasManager *_mgr;
  AliasItem *_rootItem;
  
  void setupModelData(AliasItem *parent) {
    QList<QVariant> columnData;
    columnData << "test";
    AliasItem *child = new AliasItem(columnData, true, parent);
    parent->appendChild(child);

    columnData.clear();
    columnData << "test2";
    child->appendChild(new AliasItem(columnData, false, child));
  }
};


AliasManager::AliasManager(QObject* parent)
  : QObject(parent) {

  _aliases = new QHash<QString, Alias*>;
  _model = new AliasModel(this);

  qDebug() << "AliasManager created with thread:" << this->thread();
}


AliasManager::~AliasManager() {
  delete _aliases;
  delete _model;
}


QAbstractItemModel* AliasManager::model() const {
  return _model;
}


Alias* AliasManager::match(const QString &name) const {
  return _aliases->value(name, 0);
}


bool AliasManager::remove(const QString &name) {
  return _aliases->remove(name);
}


bool AliasManager::add(const QString& name, const QString &command,
		       const QString &group) {
  _mutex.lock();
  Alias *alias = new Alias;
  alias->name = name;
  alias->command = command;
  alias->group = group;
  
  _aliases->insert(name, alias);
  _mutex.unlock();
  return true;
}


bool AliasManager::loadSettings(const QHash<QString, QVariant> &hash) {

  qDebug() << "* AliasManager loading"
	   << hash.value("aliases/size", 0).toInt() << "aliases";

  //qDebug() << hash;

  int size = hash.value("aliases/size", 0).toInt();
  for (int i = 1; i <= size; i++) {
    QString prefix = "aliases/" + QString::number(i);

    // Valid only if it contains these parts
    if (hash.contains(prefix+"/command")) {

      // Clean up the command (remove indentation, etc)
      QStringList commandList;
      foreach(QString line, hash.value(prefix+"/command").toString().trimmed()
	      .split("\n", QString::SkipEmptyParts)) {
	commandList << line.trimmed();
      }
      
      // Add the alias
      add(hash.value(prefix+"/name", QString::number(i)).toString(),
	  commandList.join("\n"),
	  hash.value(prefix+"/group", "Default").toString());
    }
    else qDebug() << "* AliasManager unable to parse alias" << i;
    
  }

  qDebug() << "* Done loading" << count() << "aliases";
  return true;
}
