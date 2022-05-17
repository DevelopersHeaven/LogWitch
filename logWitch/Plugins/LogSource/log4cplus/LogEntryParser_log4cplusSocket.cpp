/*
 * LogEntryParser_log4cplusSocket.cpp
 *
 *  Created on: May 23, 2011
 *      Author: sven
 */

#include "Plugins/LogSource/log4cplus/LogEntryParser_log4cplusSocket.h"

#include <algorithm>
#include <iostream>

#include <log4cplus/socketappender.h>
#include <log4cplus/version.h>
#include <log4cplus/spi/loggingevent.h>

#if LOG4CPLUS_VERSION >= 2000000
#define LOG4CPLUSV2
#endif

#ifdef LOG4CPLUSV2
#include <log4cplus/helpers/timehelper.h>
#endif

#include <log4cplus/loglevel.h>

#include <QtCore/QtCore>

#include "LogData/LogEntry.h"
#include "LogData/LogEntryAttributeNames.h"
#include "LogData/LogEntryFactory.h"
#include "LogData/LogEntryParserModelConfiguration.h"

#include "Plugins/LogSource/log4cplus//EntryToTextFormaterLog4cplus.h"

using namespace logwitch::plugins::log4cplus;

LogEntryParser_log4cplusSocket::LogEntryParser_log4cplusSocket (int port)
  : m_port(port), myFactory(new LogEntryFactory)
  , m_loglevelStringOff("OFF")
  , m_loglevelStringFatal("FATAL")
  , m_loglevelStringError("ERROR")
  , m_loglevelStringWarn("WARN")
  , m_loglevelStringInfo("INFO")
  , m_loglevelStringDebug("DEBUG")
  , m_loglevelStringTrace("TRACE")
  , m_name("Log4cplus Listener Port " + QString::number(port))
  , m_logEntryNumber(0), m_messageInProgress(false)
  , m_emittingAllowed(false)
{
  // Preparing attributes in factory
  LogEntryAttributeNames names;
  myFactory->addField(names.getConfiguration("number"));
  myFactory->addField(names.getConfiguration("timestamp"));
  myFactory->addField(names.getConfiguration("message"));
  myFactory->addField(names.getConfiguration("level"));
  myFactory->addField(names.getConfiguration("logger"));
  myFactory->addField(names.getConfiguration("fsource"));
  myFactory->addField(names.getConfiguration("thread"));
  myFactory->addField(names.getConfiguration("ndc"));
  myFactory->disallowAddingFields();

  m_myModelConfig = std::shared_ptr<LogEntryParserModelConfiguration>(
      new LogEntryParserModelConfiguration("log4cplus", myFactory));
  m_myModelConfig->setHierarchySplitString(4, "\\.");
  m_myModelConfig->setHierarchySplitString(5, "/");
  m_myModelConfig->setEntryToTextFormater(
      std::shared_ptr<EntryToTextFormater>(new EntryToTextFormaterLog4cplus));

  for (int i = 0; i < myFactory->getNumberOfFields(); ++i)
  {
    const AttributeConfiguration &cfg = myFactory->getFieldConfiguration(i);
    m_myModelConfig->setFieldWidthHint(i, cfg.defaultCellWidth, true);
  }
  m_myModelConfig->setFieldOrderHint({0, 7, 1, 2, 3, 4, 5, 6}, true);

  connect(this, SIGNAL(newConnection()), this, SLOT(newIncomingConnection()));
}

LogEntryParser_log4cplusSocket::~LogEntryParser_log4cplusSocket ()
{
  qDebug() << "called: ~LogEntryParser_log4cplusSocket";
  close();
  qDebug() << "finished: ~LogEntryParser_log4cplusSocket";
}

void LogEntryParser_log4cplusSocket::logEntryMessageDestroyed ()
{
  QMutexLocker lo(&m_mutex);

  if (m_nextMessage)
  {
    connect(m_nextMessage.get(), SIGNAL(destroyed(QObject *)), this,
            SLOT(logEntryMessageDestroyed()));
    TSharedNewLogEntryMessage messageToSend(m_nextMessage);
    m_nextMessage.reset();

    // Unlock and send message now.
    lo.unlock();
    emit newEntry(messageToSend);
  }
  else
  {
    m_messageInProgress = false;
  }
}

void LogEntryParser_log4cplusSocket::newEntryFromReceiver (
    std::list<TSharedLogEntry> entries)
{
  qDebug() << "New Messages received: " << entries.size();
  if (!m_emittingAllowed)
  {
    // Emitting not set up, so dropping entries.
    return;
  }
  std::list<TSharedLogEntry>::iterator it;
  for (it = entries.begin(); it != entries.end(); ++it)
    (*it)->setAttribute(QVariant(m_logEntryNumber.fetchAndAddAcquire(1)), 0);

  QMutexLocker lo(&m_mutex);

  if (m_messageInProgress)
  {
    qDebug() << "msg in progress";
    if (!m_nextMessage)
      m_nextMessage.reset(new NewLogEntryMessage);

    m_nextMessage->entries.insert(m_nextMessage->entries.end(), entries.begin(),
                                  entries.end());
  }
  else
  {
    qDebug() << "new message";
    TSharedNewLogEntryMessage newEntryMessage(new NewLogEntryMessage);
    newEntryMessage->entries = entries;
    connect(newEntryMessage.get(), SIGNAL(destroyed(QObject *)), this,
            SLOT(logEntryMessageDestroyed()));
    m_messageInProgress = true;

    lo.unlock();
    emit newEntry(newEntryMessage);
  }
}

std::shared_ptr<LogEntryParserModelConfiguration> LogEntryParser_log4cplusSocket::getParserModelConfiguration () const
{
  return m_myModelConfig;
}

bool LogEntryParser_log4cplusSocket::initParser ()
{
  qDebug() << "Server listening on port: " << m_port;
  if (!listen(QHostAddress::Any, m_port))
  {
    m_initError = tr(
        QString(
            "Listening on port " + QString::number(m_port) + " failed: "
                + errorString()).toLatin1());
    return false;
  }

  return true;
}

void LogEntryParser_log4cplusSocket::startEmiting ()
{
  m_emittingAllowed = true;
}

void LogEntryParser_log4cplusSocket::newIncomingConnection ()
{
  qDebug() << "Incoming connection ... creating new receiver.";

  QTcpSocket *socket = nextPendingConnection();
  LogEntryParser_log4cplusSocket_Receiver *receiver =
      new LogEntryParser_log4cplusSocket_Receiver(this, socket);

  connect(this, SIGNAL(destroyed()), receiver, SLOT(shutdown()));
  connect(receiver, SIGNAL(newEntry(std::list<TSharedLogEntry>)), this,
          SLOT(newEntryFromReceiver(std::list<TSharedLogEntry>)));
}

QString LogEntryParser_log4cplusSocket::getName () const
{
  return m_name;
}

LogEntryParser_log4cplusSocket_Receiver::LogEntryParser_log4cplusSocket_Receiver (
    LogEntryParser_log4cplusSocket *server, QTcpSocket *socket)
  : m_socket(socket)
  , m_bytesNeeded(0)
  , m_stateReadSize(true)
  , m_server(server)
{
  qDebug() << "new receiver created";
  m_socket->setParent(this);
  connect(m_socket, SIGNAL(readyRead()), this, SLOT(newDataAvailable()));
  connect(m_socket, SIGNAL(disconnected()), this, SLOT(shutdown()));

  // Send a byte back to give a response
  std::string data("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
  m_socket->write(data.c_str(), data.length());
}

LogEntryParser_log4cplusSocket_Receiver::~LogEntryParser_log4cplusSocket_Receiver ()
{
  qDebug() << "receiver destroyed";
}

void LogEntryParser_log4cplusSocket_Receiver::newDataAvailable ()
{
  unsigned int sizeToReadNext = 0;

  if (!m_buffer)
  {
    m_stateReadSize = true;
    m_bytesNeeded = sizeof(unsigned int);
    m_buffer.reset(new ::log4cplus::helpers::SocketBuffer(m_bytesNeeded));
  }

  std::list<TSharedLogEntry> entries;

  while (m_socket->bytesAvailable())
  {
    // read outstanding data
    readDataToBuffer();

    if (m_bytesNeeded == 0)
    {
      // interprete data
      if (m_stateReadSize)
      {
        sizeToReadNext = m_buffer->readInt();
        // Ignore message packets with 0 byte length. This is used for tunnel probing.
        if (sizeToReadNext == 0)
        {
          sizeToReadNext = sizeof(unsigned int);
        }
        else
          m_stateReadSize = false;
      }
      else
      {
        entries.push_back(bufferToEntry());

        m_stateReadSize = true;
        sizeToReadNext = sizeof(unsigned int);
      }

      // We will shutdown if we receive more then 1M of data.
      // This is a prevention from memory errors due to wrong data.
      if (sizeToReadNext > 1024 * 1024)
      {
        // Purge entries before we will shutdown the connection.
        if (!entries.empty())
          emit newEntry(entries);

        shutdown();
        return;
      }

      m_buffer.reset(new ::log4cplus::helpers::SocketBuffer(sizeToReadNext));
      m_bytesNeeded = sizeToReadNext;
    }
  }

  if (!entries.empty())
    emit newEntry(entries);
}

TSharedLogEntry LogEntryParser_log4cplusSocket_Receiver::bufferToEntry ()
{
  ::log4cplus::spi::InternalLoggingEvent event = readFromBuffer(*m_buffer);

#if QT_VERSION > 0x040700 //needs > Qt.4.7
# ifdef LOG4CPLUSV2
  using namespace ::log4cplus::helpers;
  QDateTime timestamp( QDateTime::fromMSecsSinceEpoch ( qint64(to_time_t (event.getTimestamp())) * 1000
          + ((qint64( microseconds_part(event.getTimestamp())/1000)%1000) ) ) );
# else
  QDateTime timestamp(
      QDateTime::fromMSecsSinceEpoch(
          qint64(event.getTimestamp().getTime()) * 1000
              + ((qint64(event.getTimestamp().usec() / 1000) % 1000))));
# endif
#else
  // This is a workaround for older QT versions (<=4.7)
# ifdef LOG4CPLUSV2
#  error Needs to be implemented if needed.
# else
  QDateTime timestamp( QDateTime::fromTime_t(0) );
  timestamp = timestamp.addMSecs(qint64(event.getTimestamp().getTime()) * 1000 + ((qint64( event.getTimestamp().usec()/1000)%1000) ) );
# endif
#endif

  TSharedLogEntry entry = m_server->myFactory->getNewLogEntry();

  QString logLevel = m_server->m_loglevelStringOff;
  if (event.getLogLevel() >= ::log4cplus::OFF_LOG_LEVEL)
    logLevel = m_server->m_loglevelStringOff;
  else if (event.getLogLevel() >= ::log4cplus::FATAL_LOG_LEVEL)
    logLevel = m_server->m_loglevelStringFatal;
  else if (event.getLogLevel() >= ::log4cplus::ERROR_LOG_LEVEL)
    logLevel = m_server->m_loglevelStringError;
  else if (event.getLogLevel() >= ::log4cplus::WARN_LOG_LEVEL)
    logLevel = m_server->m_loglevelStringWarn;
  else if (event.getLogLevel() >= ::log4cplus::INFO_LOG_LEVEL)
    logLevel = m_server->m_loglevelStringInfo;
  else if (event.getLogLevel() >= ::log4cplus::DEBUG_LOG_LEVEL)
    logLevel = m_server->m_loglevelStringDebug;
  else if (event.getLogLevel() >= ::log4cplus::TRACE_LOG_LEVEL)
    logLevel = m_server->m_loglevelStringTrace;

  entry->setAttribute(QVariant(timestamp), 1);
  entry->setAttribute(QVariant(QString(event.getMessage().c_str())), 2);
  entry->setAttribute(QVariant(logLevel), 3);
  entry->setAttribute(QVariant(QString(event.getLoggerName().c_str())), 4);
  entry->setAttribute(
      QVariant(
          QString(event.getFile().c_str()) + ":"
              + QString::number(event.getLine())),
      5);
  entry->setAttribute(QVariant(QString(event.getThread().c_str())), 6);
  entry->setAttribute(QVariant(QString(event.getNDC().c_str())), 7);
  return entry;
}

void LogEntryParser_log4cplusSocket_Receiver::readDataToBuffer ()
{
  const qint64 toRead = std::min<qint64>(m_socket->bytesAvailable(),
                                         m_bytesNeeded);
  size_t position = m_buffer->getMaxSize() - m_bytesNeeded;

  qint64 res = m_socket->read(m_buffer->getBuffer() + position, toRead);
  m_bytesNeeded -= res;
}

void LogEntryParser_log4cplusSocket_Receiver::shutdown ()
{
  if (m_socket->isValid() && m_socket->isOpen())
  {
    m_socket->close();
  }
  deleteLater();
}

