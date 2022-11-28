/*
 * LogEntryParser_log4cplusSocket.h
 *
 *  Created on: May 23, 2011
 *      Author: sven
 */

#ifndef LOGENTRYPARSER_LOG4CPLUSSOCKET_H_
#define LOGENTRYPARSER_LOG4CPLUSSOCKET_H_
#include <list>

#include <QAtomicInt>
#include <QtCore/QtCore>
#include <QtNetwork>

#include "LogData/LogEntryFactory.h"
#include "LogData/LogEntryParser.h"

namespace log4cplus::helpers {
class SocketBuffer;
}

namespace logwitch { namespace plugins { namespace log4cplus {


class LogEntryParser_log4cplusSocket_Receiver;

class LogEntryParser_log4cplusSocket
  : public QTcpServer
  , public LogEntryParser
{
	Q_OBJECT

	friend class LogEntryParser_log4cplusSocket_Receiver;
public:
	LogEntryParser_log4cplusSocket( int port );

	virtual ~LogEntryParser_log4cplusSocket();

	QString getName() const;

  bool initParser();

	void startEmiting();

	std::shared_ptr<LogEntryParserModelConfiguration> getParserModelConfiguration() const;

private slots:
	void newIncomingConnection();
	void newEntryFromReceiver( std::list<TSharedLogEntry> entry);

	void logEntryMessageDestroyed();

signals:
	void newEntry( TconstSharedNewLogEntryMessage );

	void signalError( QString error );

  void finished();

private:
	int m_port;

	std::shared_ptr<LogEntryFactory> myFactory;

	std::shared_ptr<LogEntryParserModelConfiguration> m_myModelConfig;

	QString m_loglevelStringOff;
	QString m_loglevelStringFatal;
	QString m_loglevelStringError;
	QString m_loglevelStringWarn;
	QString m_loglevelStringInfo;
	QString m_loglevelStringDebug;
	QString m_loglevelStringTrace;

	QString m_name;

	QAtomicInt m_logEntryNumber;

	// Mutex for protecting nextMessage and messageInProgress
    mutable QMutex m_mutex;

	TSharedNewLogEntryMessage m_nextMessage;

	bool m_messageInProgress;

	bool m_emittingAllowed;
};

class LogEntryParser_log4cplusSocket_Receiver
	: public QObject
{
	  Q_OBJECT
public:
	LogEntryParser_log4cplusSocket_Receiver( LogEntryParser_log4cplusSocket *server, QTcpSocket *socket );
	~LogEntryParser_log4cplusSocket_Receiver();

signals:
	void error(QTcpSocket::SocketError socketError);

	void newEntry( std::list<TSharedLogEntry> );

public slots:
	void newDataAvailable();

	void shutdown();
private:
	TSharedLogEntry bufferToEntry();

	void readDataToBuffer( );
private:
	QTcpSocket *m_socket;

	std::unique_ptr<::log4cplus::helpers::SocketBuffer> m_buffer;
	quint64 m_bytesNeeded;
	bool m_stateReadSize;

	LogEntryParser_log4cplusSocket *m_server;
};

}}}

#endif /* LOGENTRYPARSER_LOG4CPLUSSOCKET_H_ */
