#ifndef MMAPPERPLUGINPARSER_H
#define MMAPPERPLUGINPARSER_H

#include "abstractparser.h"

class AbstractParser;
class MapperManager;
class MMapperPlugin;

class MMapperPluginParser : public AbstractParser {
    Q_OBJECT

    public:
    MMapperPluginParser(MapperManager*, QObject *parent=0);
    ~MMapperPluginParser();

    //void parseNewMudInput(IncomingData& que) {} // Unused

public slots:
    void name(QString);
    void description(const QString &);
    void dynamicDescription(const QString &);
    void prompt(QString);
    void exits(QString);
    void move(const QString &);
    void terrain(const QString &);
    
    void userInput(QString);
    void mudOutput(const QString&);

   private:
    QString _session;
    CommandIdType _move;

    void submit();

 signals:
    //void sendToUser(const QByteArray &);
    //void sendToMud(const QByteArray &);
    //void foundDoors(const QStringList &);

};

#endif /* MMAPPERPLUGINPARSER_H */
