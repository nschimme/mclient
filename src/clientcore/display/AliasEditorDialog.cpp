#include "AliasEditorDialog.h"
#include "AliasManager.h"
#include "ObjectViewer.h"

#include <QSplitter>
#include <QWidget>
#include <QSize>

AliasEditorDialog::AliasEditorDialog(AliasManager *mgr, QWidget *parent)
  : QDialog(parent), _mgr(mgr) {
  setupUi(this);

  ObjectViewer *objectViewer = new ObjectViewer(mgr);

  // TODO: fix this!
  gridLayout->addWidget(objectViewer);

  //splitter->moveSplitter(splitter->indexOf(objectViewer), objectViewer->size().width());

  //treeView->setUniformRowHeights(true);
  //treeView->setRootIsDecorated(false);
  //treeView->setItemsExpandable(true);

}

AliasEditorDialog::~AliasEditorDialog() {}

QSize AliasEditorDialog::sizeHint() const {
  return QSize(0, 100);
}
