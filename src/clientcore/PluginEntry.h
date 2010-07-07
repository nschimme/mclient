#ifndef PLUGINENTRY_H
#define PLUGINENTRY_H

#include <QHash>
#include <QString>

class QStringList;

class PluginEntry {

    public:
        PluginEntry();
        ~PluginEntry();

        // The library filename relative to plugins dir
        const QString& libName() const;
        void libName(const QString);

        // The short name of the plugin used in hashes and maps
        const QString& shortName() const;
        void shortName(const QString);

        // The long name of the plugin displayed in the gui
        const QString& longName() const;
        void longName(const QString);
        
        // APIs implemented
        const QStringList apiList() const;
        void addAPI(const QString, const int);

        // Returns the version of a particular api.  NOTE: count starts at 1!
        // This function will return 0 if the api is not implemented.
        int version(const QString& api) const;

    protected:
        QString _shortName;
        QString _longName;
        QString _libName;

        QHash<QString, int> _apiVersions;
};


#endif // PLUGINENTRY_H
