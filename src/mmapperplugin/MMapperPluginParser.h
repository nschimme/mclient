#ifndef MMAPPERPLUGINPARSER_H
#define MMAPPERPLUGINPARSER_H

#include "abstractparser.h"

class AbstractParser;
class MapperManager;

class MMapperPluginParser : public AbstractParser {
    Q_OBJECT

    public:
    MMapperPluginParser(QString, MapperManager*, QObject *parent=0);
    ~MMapperPluginParser();

    void parseNewMudInput(IncomingData& que) {} // Unused

public slots:
    void name(QString, const QString &);
    void description(const QString &, const QString &);
    void dynamicDescription(const QString &, const QString &);
    void prompt(QString, const QString &);
    void exits(QString, const QString &);
    void move(QString, const QString &);
    
    void userInput(const QString&, const QString&);
    void mudOutput(const QString&, const QString&);

   private:
    QString _session;
    CommandIdType _move;

    void submit();

};

#endif /* MMAPPERPLUGINPARSER_H */
