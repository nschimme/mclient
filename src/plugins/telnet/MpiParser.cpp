#include "MpiParser.h"
#include <QChar>
#include <QDebug>

MpiParser::MpiParser() {
  // MUME MPI
  _mpiMode = UNKNOWN;
  _mpiState = GOT_N;
  _mpiLength = 0;
}

MpiParser::~MpiParser() {
}

bool MpiParser::isMpi(const char &input, QByteArray &cleanData) {
  switch (input) {
  case '\n':
    // Enable gotR state if we aren't in MPI-parsing-state yet
    switch (_mpiState) {
    case NORMAL:
      qDebug() << "got \\n!" << _mpiBuffer;
      _mpiState = GOT_N;
      break;

    case GOT_R:
      qDebug() << "got \\r\\n!" << _mpiBuffer << " " << _mpiState;
      _mpiState = GOT_N;
      cleanData.append(_mpiBuffer);
      _mpiBuffer.clear();
      qDebug() << cleanData;
      return false;

    case CHECK_MPI:
      if (_mpiBuffer.startsWith(MPI)) {
	qDebug() << "THIS IS MPI!!!! \\n";
	if (parseMessage()) {
	  _mpiState = PARSE_MPI;
	  qDebug() << "IN MPI STATE... OH NOES!";
	} else {
	  qDebug() << "MPI WAS INVALID!!!!";
	}
      }
      break;

    case PARSE_MPI:
      _mpiState = PARSE_TITLE;
      qDebug() << "THIS IS FIRST PARSE:" << _mpiBuffer;
      break;

    case PARSE_TITLE:
      _mpiState = PARSE_BODY;
      parseTitle();
      qDebug() << "THIS IS TITLE" << _mpiTitle;
      break;

    case PARSE_BODY:
      if (_mpiBuffer.size() >= _mpiLength) {
	_mpiState = NORMAL;
	qDebug() << "THIS IS BODY" << _mpiBuffer;
	parseBody(cleanData);
	
      } else {
	qDebug() << "FOUND MORE BODY" << _mpiLength << _mpiBuffer.size() << _mpiBuffer;
	_mpiBuffer.append(input);
      }
      break;

    default:
      return false;
    };
    break;
    
  case '\r':
    switch (_mpiState) {
    case NORMAL:
    case GOT_N:
      qDebug() << "got \\r!";
      _mpiState = GOT_R;
      return false;
    default:
      return true;
    };
    break;

  default:
    switch (_mpiState) {
    case CHECK_MPI:
      qDebug() << "got data for N!"  nput;
      _mpiBuffer.append(input);
      break;
    case PARSE_MPI:
      qDebug() << "IN PARSE MPI STATE";
      _mpiState = PARSE_TITLE;
    case PARSE_BODY:
    case PARSE_TITLE:
      qDebug() << "got data for N!"  nput;
      _mpiBuffer.append(input);	
      break;
    case GOT_N:
    case GOT_R:
      if (_mpiBuffer.length() > MPILEN) {
	// Obviously not MPI
	cleanData.append(_mpiBuffer);
	_mpiBuffer.clear();
	qDebug() << "message too long" << cleanData;
	_mpiState = NORMAL;
	break;
	
      } else if (_mpiBuffer.length() == MPILEN && _mpiBuffer == MPI) {
	qDebug() << "CHECK_MPI";
	_mpiState = CHECK_MPI;
	
      }
      qDebug() << "got data for N!" << input;
      _mpiBuffer.append(input);	
      return true;

    default:
      return false;
    }
    break;
  };
  return true;
}

bool MpiParser::parseMessage() {
  _mpiLength = 0;

  switch (_mpiBuffer.at(4)) {
  case 'E':
    _mpiMode = EDITOR;
    break;
  case 'V':
    _mpiMode = VIEWER;
    break;
  default:
    qDebug() << "! Unknown MPI mode: " << _mpiBuffer.at(5);
    _mpiMode = UNKNOWN;
    return false;
  };

  QByteArray length(_mpiBuffer.mid(5));
  _mpiLength = length.toInt() - _mpiBuffer.length();

  _mpiBuffer.clear();
  return true;
}


bool MpiParser::parseTitle() {
  _mpiTitle = _mpiBuffer;
  _mpiLength -= _mpiTitle.length();
  _mpiBuffer.clear();
  return true;
}

bool MpiParser::parseBody(QByteArray &cleanData) {
  QByteArray xml;

  switch (_mpiMode) {
  case EDITOR:
    xml.append("<edit key=");
    xml.append(_mpiTitle);
    xml.append("><title>");
    xml.append(_mpiTitle);
    xml.append("</title><body>");
    xml.append(_mpiBuffer.toBase64());
    xml.append("</body></edit>");
    break;
  case VIEWER:
    xml.append("<view><title>");
    xml.append(_mpiTitle);
    xml.append("</title><body>");
    xml.append(_mpiBuffer.toBase64());
    xml.append("</body></view>");
    break;
  default:
    return false;
  }
  _mpiBuffer.clear();
  qDebug() << "XML: " << xml;
  cleanData.append(xml);
  qDebug() << cleanData;
  return true;
}
