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

#include "LogData/LogEntryParser.h"

class LogEntryFactory;

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

	~LogEntryParser_log4cplusSocket() override;

	QString getName() const override;

	bool initParser() override;

	void startEmiting() override;

	std::shared_ptr<LogEntryParserModelConfiguration> getParserModelConfiguration() const override;

private slots:
	void newIncomingConnection();
	void newEntryFromReceiver( std::list<TSharedLogEntry> entry);

	void logEntryMessageDestroyed();

signals:
	void newEntry(TconstSharedNewLogEntryMessage) override;

	void signalError(QString error) override;

	void finished() override;

private:
	int m_port;

	std::shared_ptr<LogEntryFactory> myFactory;

	std::shared_ptr<LogEntryParserModelConfiguration> m_myModelConfig;

	QString m_loglevelStringOff = "OFF";
	QString m_loglevelStringFatal = "FATAL";
	QString m_loglevelStringError = "ERROR";
	QString m_loglevelStringWarn = "WARN";
	QString m_loglevelStringInfo = "INFO";
	QString m_loglevelStringDebug = "DEBUG";
	QString m_loglevelStringTrace = "TRACE";

	QString m_name;

	QAtomicInt m_logEntryNumber = 0;

	// Mutex for protecting nextMessage and messageInProgress
    mutable QMutex m_mutex;

	TSharedNewLogEntryMessage m_nextMessage;

	bool m_messageInProgress = false;

	bool m_emittingAllowed = false;
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
	quint64 m_bytesNeeded = 0;
	bool m_stateReadSize = true;

	LogEntryParser_log4cplusSocket *m_server;
};

}}}

#endif /* LOGENTRYPARSER_LOG4CPLUSSOCKET_H_ */
