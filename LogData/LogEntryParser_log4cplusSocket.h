/*
 * LogEntryParser_log4cplusSocket.h
 *
 *  Created on: May 23, 2011
 *      Author: sven
 */

#ifndef LOGENTRYPARSER_LOG4CPLUSSOCKET_H_
#define LOGENTRYPARSER_LOG4CPLUSSOCKET_H_
#include <list>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <log4cplus/helpers/socket.h>

#include <QAtomicInt>
#include <QtCore/QtCore>
#include <QtNetwork>

#include "LogData/LogEntryFactory.h"
#include "LogData/LogEntryParser.h"

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

	void startEmiting();

	boost::shared_ptr<LogEntryParserModelConfiguration> getParserModelConfiguration() const;

private slots:
	void newIncomingConnection();
	void newEntryFromReceiver( std::list<TSharedLogEntry> entry);

	void logEntryMessageDestroyed();

signals:
	void newEntry( TconstSharedNewLogEntryMessage );

	void signalError( QString error );

private:
	int m_port;

	boost::shared_ptr<LogEntryFactory> myFactory;

	boost::shared_ptr<LogEntryParserModelConfiguration> m_myModelConfig;

	boost::shared_ptr<QString> m_loglevelStringOff;
	boost::shared_ptr<QString> m_loglevelStringFatal;
	boost::shared_ptr<QString> m_loglevelStringError;
	boost::shared_ptr<QString> m_loglevelStringWarn;
	boost::shared_ptr<QString> m_loglevelStringDebug;
	boost::shared_ptr<QString> m_loglevelStringTrace;

	QString m_name;

	QAtomicInt m_logEntryNumber;

	// Mutex for protecting nextMessage and messageInProgress
    mutable QMutex m_mutex;

	TSharedNewLogEntryMessage m_nextMessage;

	bool m_messageInProgress;
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

	boost::shared_ptr<log4cplus::helpers::SocketBuffer> m_buffer;
	quint64 m_bytesNeeded;
	bool m_stateReadSize;

	LogEntryParser_log4cplusSocket *m_server;
};

#endif /* LOGENTRYPARSER_LOG4CPLUSSOCKET_H_ */
