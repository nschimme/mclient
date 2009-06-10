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
    
    // Create Splash
    QSplashScreen *splash =
      new QSplashScreen(QPixmap(":/mainwindow/intro.png"));
    splash->show();
    splash->showMessage("Loading plugins...",
			Qt::AlignBottom | Qt::AlignHCenter,
			Qt::white);

    // Create Singletons
    ConfigManager *cnfmgr = ConfigManager::instance();
    qDebug() << "dumdedumdum";
    PluginManager *plgmgr = PluginManager::instance();
    CommandManager *cmdmgr = CommandManager::instance();
    MainWindow *mw = MainWindow::instance();
    
    // Start mClient by loading plugins
    plgmgr->start(QThread::TimeCriticalPriority);

    // MainWindow controls the splash
    splash->finish(mw);
    delete splash;

    return app.exec();
}
