#include "CommandEntry.h"

CommandEntry::CommandEntry() {
}


CommandEntry::~CommandEntry() {
}


const QString& CommandEntry::pluginName() const {
    return _pluginName;
}


void CommandEntry::pluginName(const QString name) {
    _pluginName = name;
}


const QString& CommandEntry::command() const {
    return _command;
}


void CommandEntry::command(const QString cmd) {
    _command = cmd;
}


const QString& CommandEntry::help() const {
    return _help;
}


void CommandEntry::help(const QString help) {
    _help = help;
}


const QString& CommandEntry::dataType() const {
    return _dataType;
}


void CommandEntry::dataType(const QString dataType) {
    _dataType = dataType;
}
