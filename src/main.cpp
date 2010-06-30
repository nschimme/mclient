#include "PluginManager.h"

#include <QApplication>
#include <stdio.h>
#include <memory>

int main(int argc, char** argv) {
    QApplication::setOrganizationName("MUME");
    QApplication::setOrganizationDomain("mume.org");
    QApplication::setApplicationName("mClient");

    QApplication app(argc, argv);
    
    // Check for command line commands that we can handle here before
    // the application executes
    for (int i = 0; i < argc; i++) {
      if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
	printf("Usage:\n"
	       "mclient --profile [profile]\n"
	       "mclient --help, -h\n"
	       );
	return 0;

      }
    }

    std::auto_ptr<PluginManager> pm (PluginManager::getInstance());
    int ret = app.exec();
    return ret;
}
