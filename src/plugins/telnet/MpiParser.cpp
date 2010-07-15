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
      //qDebug() << "got \\n!" << _mpiBuffer;
      _mpiState = GOT_N;
      break;

    case GOT_R:
      //qDebug() << "got \\r\\n!" << _mpiBuffer << " " << _mpiState;
      _mpiState = GOT_N;
      cleanData.append(_mpiBuffer);
      _mpiBuffer.clear();
      return false;

    case CHECK_MPI:
      if (_mpiBuffer.startsWith(MPI)) {
	qDebug() << "THIS IS MPI!!!! \\n";
	if (!parseMessage()) {
	  qDebug() << "GOT INVALID MPI";
	  // TODO: append cleanData?
	  _mpiState = NORMAL;
	}
      }
      break;

    case PARSE_KEY:
      parseKey();
      qDebug() << "THIS IS KEY" << _mpiKey;
      break;

    case PARSE_TITLE:
      parseTitle();
      qDebug() << "THIS IS TITLE" << _mpiTitle;
      if (_mpiBuffer.size() >= _mpiLength) {
	_mpiState = NORMAL;
	qDebug() << "THERE IS NO BODY!";
	parseBody(cleanData);
      }
      break;

    case PARSE_BODY:
      if (_mpiBuffer.size() >= _mpiLength) {
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
      //qDebug() << "got \\r!";
      _mpiState = GOT_R;
      return false;
    default:
      return true;
    };
    break;

  default:
    switch (_mpiState) {
    case CHECK_MPI:
      qDebug() << "CHECK_MPI got data for N!" << input;
      _mpiBuffer.append(input);
      break;

    case PARSE_KEY:
    case PARSE_BODY:
    case PARSE_TITLE:
      //qDebug() << "PARSE_BODY/TITLE/KEY got data for N!" << input;
      _mpiBuffer.append(input);	
      break;

    case GOT_N:
    case GOT_R:
      if (_mpiBuffer.length() > MPILEN) {
	// Obviously not MPI
	qDebug() << "message too long" << _mpiBuffer;
	cleanData.append(_mpiBuffer);
	_mpiBuffer.clear();
	_mpiState = NORMAL;
	return false;
	
      } else if (_mpiBuffer.length() == MPILEN && _mpiBuffer == MPI) {
	qDebug() << "CHECK_MPI" << _mpiBuffer;
	_mpiState = CHECK_MPI;
	
      }
      //qDebug() << "got data for N!" << input;
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
    _mpiState = PARSE_KEY;
    break;
  case 'V':
    _mpiMode = VIEWER;
    _mpiState = PARSE_TITLE;
    break;
  default:
    qDebug() << "! Unknown MPI mode: " << _mpiBuffer.at(5);
    _mpiMode = UNKNOWN;
    _mpiState = GOT_N;
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
  _mpiState = PARSE_BODY;
  return true;
}

bool MpiParser::parseKey() {
  if (_mpiBuffer.at(0) == 'M') {
    _mpiKey = _mpiBuffer.mid(1);
    _mpiLength -= _mpiBuffer.length();
    _mpiBuffer.clear();
    _mpiState = PARSE_TITLE;
    return true;
  }
  // TODO:: append _cleanData?
  _mpiKey = "-1";
  return false;
}

bool MpiParser::parseBody(QByteArray &cleanData) {
  QByteArray xml;

  switch (_mpiMode) {
  case EDITOR:
    xml.append("<edit key=");
    xml.append(_mpiKey);
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
  _mpiTitle.clear();
  _mpiKey.clear();
  qDebug() << "Output XML for remote editing: " << xml;
  cleanData.append(xml);
  _mpiState = NORMAL;
  return true;
}
