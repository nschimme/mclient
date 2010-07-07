#ifndef PLUGINCONFIGWIDGET_H
#define PLUGINCONFIGWIDGET_H

#include <QWidget>

#include <QHash>

class QItemSelection;
class QModelIndex;
class QPluginLoader;
class QPushButton;
class QString;
class QTreeView;
class QTextEdit;


class PluginConfigWidget : public QWidget {
    Q_OBJECT
    
    public:
        PluginConfigWidget(QHash<QString, QPluginLoader*> plugins,
                QWidget* parent=0);
        ~PluginConfigWidget();

    public slots:
        void on_configure();
        void updateSelection(const QItemSelection &selected,
            const QItemSelection &deselected);

    private:
        QHash<QString, QPluginLoader*> _plugins;
        QPushButton* _configButton;
        QTreeView* _tree;
        QTextEdit* _descBox;

};


#endif /* PLUGINCONFIGWIDGET_H */
