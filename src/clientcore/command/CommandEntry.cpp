#include "CommandEntry.h"

CommandEntry::CommandEntry(const QString command, const QString help,
			   CommandEntryType commandType) {
  _command = command;
  _help = help;
  _commandType = commandType;
  _locking = false;

}

CommandEntry::CommandEntry() {
  _command = "unknown";
  _commandType = CMD_ONE_LINE;
  _locking = false;

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


CommandEntryType CommandEntry::commandType() const {
  return _commandType;
}


void CommandEntry::commandType(CommandEntryType commandType) {
  _commandType = commandType;
}


bool CommandEntry::locking() const {
  return _locking;
}


void CommandEntry::locking(bool value) {
  _locking = value;
}
