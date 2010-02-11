#include "MumeXML.h"

#include "PluginSession.h"
#include "EventHandler.h"

#include <QDebug>

Q_EXPORT_PLUGIN2(mumexml, MumeXML)


MumeXML::MumeXML(QObject* parent) 
        : MClientPlugin(parent) {
    _shortName = "mumexmlfilter";
    _longName = "Mume XML Filter";
    _description = "Filters the XML tags.";
    _dependencies.insert("telnet", 1);
    _implemented.insert("mumexml",1);
    _receivesDataTypes.insert("TelnetData", 3);
    _receivesDataTypes.insert("TelnetGA", 3);
    _receivesDataTypes.insert("SocketDisconnected", 3);
    _receivesDataTypes.insert("SocketConnected", 3);
    /*
    _deliversDataTypes << "XMLNone" << "XMLAll" << "XMLTag"

		       << "XMLPrompt" << "XMLRoom" << "XMLDescription"
		       << "XMLDynamicDescription" << "XMLExits"

		       << "XMLTerrain" << "XMLMagic" << "XMLWeather"

		       << "XMLTell" << "XMLSay" << "XMLNarrate" << "XMLSong"
		       << "XMLPray" << "XMLShout" << "XMLYell" << "XMLEmote"
		       << "XMLCommunication"
      
		       << "XMLHit" << "XMLDamage" << "XMLCombat";
    */
}


MumeXML::~MumeXML() {
  qDebug() << "~MumeXML";
}


void MumeXML::configure() {
}


bool MumeXML::startSession(PluginSession *ps) {
  _eventHandlers[ps->session()] = new EventHandler(ps, this);
  return true;
}


bool MumeXML::stopSession(PluginSession *ps) {
  _eventHandlers[ps->session()]->deleteLater();
  _eventHandlers.remove(ps->session());
  return true;
}


MClientEventHandler* MumeXML::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}
