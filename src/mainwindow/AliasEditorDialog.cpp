#include "AliasEditorDialog.h"
#include "AliasManager.h"

#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>
#include <QSize>

class ObjectView : public QTreeView {
public:
  ObjectView(QWidget *parent=0)
    : QTreeView(parent) {};
  
  virtual ~ObjectView() {};
  
  QSize sizeHint() const {
    return QSize(0, 100);
  };
};

AliasEditorDialog::AliasEditorDialog(AliasManager *mgr, QWidget *parent)
  : QDialog(parent), _mgr(mgr) {
  setupUi(this);

  ObjectView *objectView = new ObjectView(this);
  objectView->setModel(_mgr->model());

  // TODO: fix this!
  gridLayout->addWidget(objectView, 1, 0, 1, 2);

  //treeView->setUniformRowHeights(true);
  //treeView->setRootIsDecorated(false);
  //treeView->setItemsExpandable(true);

}

AliasEditorDialog::~AliasEditorDialog() {}

