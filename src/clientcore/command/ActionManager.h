#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QStringList>
#include <QMultiHash>
#include <QMultiMap>
#include <QRegExp>
#include <QMutex>
#include <QVariant>

typedef struct Action {
  QString label;
  QRegExp pattern;
  QString text;
  QString command;
  QString group;
  QStringList tags;
  uint priority;
  bool active;
  bool substitute;
} Action;


//class QAbstractTableModel;
//class ActionModel;

class ActionManager : public QObject {
    Q_OBJECT

 public:
    ActionManager(QObject* parent=0);
    ~ActionManager();

    Action* match(const QString&, const QStringList&) const;
    bool add(const QString&, const QRegExp&, const QString&,
	     const QStringList& =QStringList("XMLNone"),
	     const QString& =0, bool =0, int =5);

    QMultiMap<int, QMultiHash<QString, Action*>* > getHash() const {
      return _actions;
    };

    bool loadSettings(const QHash<QString, QVariant> &);

 protected:
    Action* add(Action *);
    
 private:
    mutable QMutex _mutex;
    QMultiMap<int, QMultiHash<QString, Action*>* > _actions;
    
};


#endif /* ACTIONMANAGER_H */
