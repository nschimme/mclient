#include "MumeXML.h"
#include "EventHandler.h"

#include <QDebug>

Q_EXPORT_PLUGIN2(mumexml, MumeXML)


MumeXML::MumeXML(QObject* parent) 
        : MClientFilterPlugin(parent) {
    _shortName = "mumexmlfilter";
    _longName = "Mume XML Filter";
    _description = "Filters the XML tags.";
    _dependencies.insert("telnet", 1);
    _implemented.insert("mumexml",1);
    _receivesDataTypes << "TelnetData";
    _deliversDataTypes << "XMLNone" << "XMLAll" << "XMLTag"

		       << "XMLPrompt" << "XMLRoom" << "XMLDescription"
		       << "XMLDynamicDescription" << "XMLExits"

		       << "XMLTerrain" << "XMLMagic" << "XMLWeather"

		       << "XMLTell" << "XMLSay" << "XMLNarrate" << "XMLSong"
		       << "XMLPray" << "XMLShout" << "XMLYell" << "XMLEmote"
		       << "XMLCommunication"
      
		       << "XMLHit" << "XMLDamage" << "XMLCombat";
}


MumeXML::~MumeXML() {
}


void MumeXML::configure() {
}


bool MumeXML::loadSettings() {
    return true;
}


bool MumeXML::saveSettings() const {
    return true;
}


bool MumeXML::startSession(QString s) {
  _eventHandlers[s] = new EventHandler;
  return true;
}


bool MumeXML::stopSession(QString s) {
  delete _eventHandlers[s];
  return true;
}


MClientEventHandler* MumeXML::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}
