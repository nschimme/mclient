#ifndef _ALIASEDITORDIALOG_H
#define _ALIASEDITORDIALOG_H

#include <QDialog>
#include "ui_AliasEditorDialog.h"

class AliasManager;
class ObjectView;

class AliasEditorDialog : public QDialog, public Ui::AliasEditorDialog
{
  Q_OBJECT
    
  public:
    AliasEditorDialog(AliasManager *mgr, QWidget *parent=0);
    virtual ~AliasEditorDialog();

    QSize sizeHint() const;

   private:
    AliasManager *_mgr;
};

#endif /* _ALIASEDITORDIALOG_H */
