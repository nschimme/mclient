#include "MainWindow.h"
#include "ConfigManager.h"
#include "QuickConnectDialog.h"

#include <QSplashScreen>
#include <QApplication>
#include <stdio.h>
#include <memory>

void usage() {
    printf("Usage:\n"
           "mclient -profile [profile]\n"
           "mclient -help, -h\n"
           );
}

int main(int argc, char** argv) {
    QApplication::setOrganizationName("MUME");
    QApplication::setOrganizationDomain("mume.org");
    QApplication::setApplicationName("mClient");

    QApplication app(argc, argv);
    
    // Check for command line commands that we can handle here before
    // the application executes
    QStringList args = QCoreApplication::arguments();
    bool showQuickConnect = false;
    QString profile;
    if (args.contains("help") || args.contains("h")) {
        usage();
        return app.exec();

    } else if (args.contains("profile")) {
        int pos = args.indexOf("profile") + 1;
        if (pos < args.size()) {
            profile = args.at(pos);
            showQuickConnect = false;
        }
    }

    // Create Splash Screen
    QSplashScreen splash(QPixmap(":/mainwindow/intro.png"));
    splash.showMessage("Loading configuration...",
                       Qt::AlignBottom | Qt::AlignHCenter,
                       Qt::white);
    splash.show();

    // Expensive start-up operations here
    ConfigManager cfg;
    MainWindow window(&cfg);

    // Construct and show a Quick Connect Dialog if no profile was provided
    if (showQuickConnect) {
        QuickConnectDialog *dlg = new QuickConnectDialog(&cfg, &window);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
        splash.finish(dlg);

    } else {
        splash.finish(&window);
        window.startProfile(profile);
        window.show();
    }
    return app.exec();
}
