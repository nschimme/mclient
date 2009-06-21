#include "PluginManager.h"

#include <QApplication>
#include <stdio.h>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QApplication::setOrganizationName("MUME");
    QApplication::setOrganizationDomain("mume.org");
    QApplication::setApplicationName("mclient");
    
    // Check for command line commands that we can handle here before
    // the application executes
    for (int i = 0; i < argc; i++) {
      if (strcmp(argv[i], "-help") == 0) {
	printf("Usage:\n"
	       "mclient -profile [profile]\n"
	       "mclient -help\n"
	       );
	return 0;

      }
    }
    
    // Create Singletons
    PluginManager::instance();

    return app.exec();
}
