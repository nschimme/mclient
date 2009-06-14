#include "PluginManager.h"
#include "MainWindow.h"

#include <QApplication>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QApplication::setOrganizationName("MUME");
    QApplication::setOrganizationDomain("mume.org");
    QApplication::setApplicationName("mclient");
    
    // Create Splash
    QSplashScreen *splash =
      new QSplashScreen(QPixmap(":/mainwindow/intro.png"));
    splash->setAttribute(Qt::WA_DeleteOnClose);
    splash->show();
    splash->showMessage("Loading plugins...",
			Qt::AlignBottom | Qt::AlignHCenter,
			Qt::white);

    // Create Singletons
    PluginManager *pluginManager = PluginManager::instance();

    // MainWindow controls the splash
    splash->finish(pluginManager->getMainWindow());

    return app.exec();
}
