#ifndef MPIPARSER_H
#define MPIPARSER_H

#include <QByteArray>
#include <QChar>

// MUME protocol introducer
#define MPI "~$#E"
#define MPILEN 4

class MpiParser {
    
    public:
         MpiParser();
        ~MpiParser();
	
	bool isMpi(const char &, QByteArray &cleanData);

	enum MpiState {NORMAL, GOT_N, GOT_R, CHECK_MPI, PARSE_MPI, PARSE_TITLE, PARSE_BODY};
	enum MpiMode {UNKNOWN, VIEWER, EDITOR};

 private:
	bool parseMessage();
	bool parseTitle();
	bool parseBody(QByteArray &cleanData);

	QByteArray _mpiBuffer, _mpiTitle;
	MpiState _mpiState;
	MpiMode _mpiMode;
	int _mpiLength;
};


#endif /* MPIPARSER_H */
