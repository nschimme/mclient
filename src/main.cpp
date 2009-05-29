#include "MainWindow.h"
#include "ConfigManager.h"
#include "PluginManager.h"
#include "CommandManager.h"

#include <QApplication>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QApplication::setOrganizationName("MUME");
    QApplication::setOrganizationDomain("mume.org");
    QApplication::setApplicationName("mclient");

    // Create Singletons
    ConfigManager *cnfmgr = ConfigManager::instance();
    PluginManager *plgmgr = PluginManager::instance();
    CommandManager *cmdmgr = CommandManager::instance();
    MainWindow *mw = MainWindow::instance();

    // Start mClient by loading plugins
    plgmgr->start(QThread::TimeCriticalPriority);

    return app.exec();
}
