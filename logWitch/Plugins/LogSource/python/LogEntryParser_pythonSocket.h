/*
 * LogEntryParser_pythonSocket.h
 *
 *  Created on: May 23, 2011
 *      Author: sven
 */

#ifndef LogEntryParser_pythonSocket_H_
#define LogEntryParser_pythonSocket_H_
#include <list>
#include <cstdint>

#include <boost/python.hpp>

#include <log4cplus/helpers/socket.h>

#include <QAtomicInt>
#include <QtCore/QtCore>
#include <QtNetwork>

#include "LogData/LogEntryFactory.h"
#include "LogData/LogEntryParser.h"

namespace logwitch { namespace plugins { namespace python {


class LogEntryParser_pythonSocket_Receiver;

class LogEntryParser_pythonSocket
  : public QTcpServer
  , public LogEntryParser
{
	Q_OBJECT

	friend class LogEntryParser_pythonSocket_Receiver;
public:
	LogEntryParser_pythonSocket( int port );

	virtual ~LogEntryParser_pythonSocket();

	QString getName() const;

  bool initParser();

	void startEmiting();

	std::shared_ptr<LogEntryParserModelConfiguration> getParserModelConfiguration() const;

private Q_SLOTS:
	void newIncomingConnection();
	void newEntryFromReceiver( std::list<TSharedLogEntry> entry);

	void logEntryMessageDestroyed();
	void onError(QString message);

Q_SIGNALS:
	void newEntry( TconstSharedNewLogEntryMessage );

	void signalError( QString error );

  void finished();

private:
	int m_port;

	std::shared_ptr<LogEntryFactory> myFactory;

	std::shared_ptr<LogEntryParserModelConfiguration> m_myModelConfig;

	QString m_name;

	QAtomicInt m_logEntryNumber;

	// Mutex for protecting nextMessage and messageInProgress
  mutable QMutex m_mutex;

	TSharedNewLogEntryMessage m_nextMessage;

	bool m_messageInProgress;

	bool m_emittingAllowed;
};

class LogEntryParser_pythonSocket_Receiver
	: public QObject
{
	  Q_OBJECT
public:
	LogEntryParser_pythonSocket_Receiver( LogEntryParser_pythonSocket *server, QTcpSocket *socket );
	~LogEntryParser_pythonSocket_Receiver();

Q_SIGNALS:
	void error(QTcpSocket::SocketError socketError);

	void error(QString error);

	void newEntry( std::list<TSharedLogEntry> );

public Q_SLOTS:
	void newDataAvailable();

	void shutdown();
private:
	TSharedLogEntry bufferToEntry();

	void readDataToBuffer( );
private:
	QTcpSocket *m_socket;

	typedef uint32_t sizePayload_t;
	std::vector<char> m_buffer;
	quint64 m_bytesNeeded;
	bool m_stateReadSize;

	LogEntryParser_pythonSocket *m_server;

	boost::python::object m_pickleImport;

	bool m_raiseErrors = true;
};

}}}

#endif /* LogEntryParser_pythonSocket_H_ */
