#ifndef ALIASMANAGER_H
#define ALIASMANAGER_H

#include <QObject>
#include <QStringList>
#include <QHash>
#include <QMutex>
#include <QVariant>

typedef struct Alias {
  QString name;
  QString command;
  QString group;
  bool active;
} Alias;

class QAbstractItemModel;
class AliasItem;
class AliasModel;

class AliasManager : public QObject {
    Q_OBJECT

 public:
    AliasManager(QObject* parent=0);
    ~AliasManager();

    Alias* match(const QString &name) const;
    bool add(const QString&, const QString&, const QString& =0);
    bool remove(const QString &name);

    int count() { return _aliases->size(); };
    QHash<QString, Alias*> getHash() const { return *_aliases; };

    bool loadSettings(const QHash<QString, QVariant> &);

    QAbstractItemModel *model () const;
    
 private:
    mutable QMutex _mutex;
    QHash<QString, Alias*> *_aliases;
    AliasModel *_model;
    
};


#endif /* ALIASMANAGER_H */
