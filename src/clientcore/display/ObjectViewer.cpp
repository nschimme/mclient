#include "ObjectViewer.h"
#include "AliasManager.h"

#include <QTreeView>
#include <QGridLayout>
#include <QToolButton>
#include <QSize>
#include <QLineEdit>
#include <QSizePolicy>


class ObjectView : public QTreeView {
public:
  ObjectView(QWidget *parent=0)
    : QTreeView(parent) {};
  
  virtual ~ObjectView() {};
  
  QSize sizeHint() const {
    return QSize(100, 0);
  };
};

ObjectViewer::ObjectViewer(AliasManager *mgr, QWidget *parent)
  : QWidget(parent), _mgr(mgr) {

  QSizePolicy policy = QSizePolicy(QSizePolicy::Minimum,
				   QSizePolicy::Expanding);
  setSizePolicy(policy);
  QWidget::setMinimumSize(50, 0);

  // Layout
  QGridLayout *gridLayout = new QGridLayout(this);

  // Object View
  ObjectView *objectView = new ObjectView(this);
  objectView->setModel(_mgr->model());

  // ToolButton
  QToolButton *toolButton = new QToolButton(this);

  // LineEdit
  QLineEdit *lineEdit = new QLineEdit(this);
  
  // TODO: fix this!
  gridLayout->addWidget(lineEdit, 0, 0);
  gridLayout->addWidget(toolButton, 0, 1);
  gridLayout->addWidget(objectView, 1, 0, 1, 2);

  //treeView->setUniformRowHeights(true);
  //treeView->setRootIsDecorated(false);
  //treeView->setItemsExpandable(true);

}

ObjectViewer::~ObjectViewer() {}

QSize ObjectViewer::sizeHint() const {
  return QSize(50, 0);
}
