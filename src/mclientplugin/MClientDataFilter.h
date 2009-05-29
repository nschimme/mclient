#ifndef MCLIENTDATAFILTER_H
#define MCLIENTDATAFILTER_H

#include <QHash>

class QEvent;
class QString;


class MClientDataFilter : public MClientPluginInterface {
    
    public:
        // The library filename relative to plugins dir
        virtual const QString& libName() const=0;

        // The short name of the plugin used in hashes and maps
        virtual const QString& shortName() const=0;

        // The long name of the plugin displayed in the gui
        virtual const QString& longName() const=0;

        // The description of the plugin shown to the user
        virtual const QString& description() const=0;

        // The plugin version string
        virtual const QString& version() const=0;

        // Returns a QStringList of APIs this plugin implements to be used
        // when checking dependencies.
        virtual const QHash<QString, int> implemented() const=0;
        
        // Returns a QStringList of APIs this plugin requires to be
        // implemented before it can be loaded.
        virtual const QHash<QString, int> dependencies() const=0;


        // Data filter members
        virtual bool event(QEvent* e);
};

Q_DECLARE_INTERFACE(MClientDataFilter,
        "mume.mclient.MClientDataFilter/1.0")

#endif /* MCLIENTDATAFILTER_H */
