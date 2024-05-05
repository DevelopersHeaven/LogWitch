/*
 * LogEntryParser_pythonSocket.cpp
 *
 *  Created on: May 23, 2011
 *      Author: sven
 */

#include "Plugins/LogSource/python/LogEntryParser_pythonSocket.h"

#include <algorithm>

#include <boost/endian/conversion.hpp>
#include <boost/python.hpp>

#include <Python.h>

#include "LogData/LogEntry.h"
#include "LogData/LogEntryAttributeNames.h"
#include "LogData/LogEntryFactory.h"
#include "LogData/LogEntryParserModelConfiguration.h"

using namespace logwitch::plugins::python;

LogEntryParser_pythonSocket::LogEntryParser_pythonSocket (int port)
  : m_port(port), myFactory(new LogEntryFactory)
  , m_name("Python Socket Receiver Port " + QString::number(port))
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
  myFactory->addField(names.getConfiguration("filename"));
  myFactory->addField(names.getConfiguration("thread"));
  myFactory->addField(names.getConfiguration("threadName"));
  myFactory->addField(names.getConfiguration("process"));
  myFactory->addField(names.getConfiguration("processName"));
  myFactory->addField(names.getConfiguration("funcName"));
  myFactory->disallowAddingFields();

  m_myModelConfig = std::shared_ptr<LogEntryParserModelConfiguration>(
      new LogEntryParserModelConfiguration("python", myFactory));
  m_myModelConfig->setHierarchySplitString(4, "\\.");
  m_myModelConfig->setHierarchySplitString(5, "/");
  m_myModelConfig->setHierarchySplitString(6, "/");

  for (int i = 0; i < myFactory->getNumberOfFields(); ++i)
  {
    const AttributeConfiguration &cfg = myFactory->getFieldConfiguration(i);
    m_myModelConfig->setFieldWidthHint(i, cfg.defaultCellWidth, true);
  }

  m_myModelConfig->setFieldOrderHint(
      {0, 7, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
      true);

  connect(this, &QTcpServer::newConnection, this, &LogEntryParser_pythonSocket::newIncomingConnection);
}

LogEntryParser_pythonSocket::~LogEntryParser_pythonSocket ()
{
  qDebug() << "called: ~LogEntryParser_pythonSocket";
  close();
  qDebug() << "finished: ~LogEntryParser_pythonSocket";
}

void LogEntryParser_pythonSocket::logEntryMessageDestroyed ()
{
  QMutexLocker lo(&m_mutex);

  if (m_nextMessage)
  {
    connect(m_nextMessage.get(), &QObject::destroyed, this,
            &LogEntryParser_pythonSocket::logEntryMessageDestroyed);
    TSharedNewLogEntryMessage messageToSend(m_nextMessage);
    m_nextMessage.reset();

    // Unlock and send message now.
    lo.unlock();
    Q_EMIT newEntry(messageToSend);
  }
  else
  {
    m_messageInProgress = false;
  }
}

void LogEntryParser_pythonSocket::newEntryFromReceiver (
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
    connect(newEntryMessage.get(), &QObject::destroyed, this,
            &LogEntryParser_pythonSocket::logEntryMessageDestroyed);
    m_messageInProgress = true;

    lo.unlock();
    Q_EMIT newEntry(newEntryMessage);
  }
}

std::shared_ptr<LogEntryParserModelConfiguration> LogEntryParser_pythonSocket::getParserModelConfiguration () const
{
  return m_myModelConfig;
}

bool LogEntryParser_pythonSocket::initParser ()
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

void LogEntryParser_pythonSocket::startEmiting ()
{
  m_emittingAllowed = true;
}

void LogEntryParser_pythonSocket::newIncomingConnection ()
{
  qDebug() << "Incoming connection ... creating new receiver.";

  QTcpSocket *socket = nextPendingConnection();
  LogEntryParser_pythonSocket_Receiver *receiver =
      new LogEntryParser_pythonSocket_Receiver(this, socket);

  connect(this, &QObject::destroyed, receiver, &LogEntryParser_pythonSocket_Receiver::shutdown);
  connect(receiver, SIGNAL(newEntry(std::list<TSharedLogEntry>)), this,
          SLOT(newEntryFromReceiver(std::list<TSharedLogEntry>)));

  // The next has to be used with a QueuedConnection, otherwise a SEGFAULT may
  // occur in case of errors. The object will also be deleted (although there
  // is a deleteLater() but the processing will jump to the event queue and BANG.
  connect(receiver, SIGNAL(error(QString)), this, SLOT(onError(QString)), Qt::QueuedConnection);
}

void LogEntryParser_pythonSocket::onError(QString message)
{
  Q_EMIT signalError(message);
}

QString LogEntryParser_pythonSocket::getName () const
{
  return m_name;
}

LogEntryParser_pythonSocket_Receiver::LogEntryParser_pythonSocket_Receiver (
    LogEntryParser_pythonSocket *server, QTcpSocket *socket)
  : m_socket(socket)
  , m_bytesNeeded(0)
  , m_stateReadSize(true)
  , m_server(server)
{
  qDebug() << "new receiver created";
  m_socket->setParent(this);
  connect(m_socket, &QIODevice::readyRead, this, &LogEntryParser_pythonSocket_Receiver::newDataAvailable);
  connect(m_socket, &QAbstractSocket::disconnected, this, &LogEntryParser_pythonSocket_Receiver::shutdown);

  m_stateReadSize = true;
  m_bytesNeeded = sizeof(sizePayload_t);
  m_buffer.clear();
  m_buffer.resize( m_bytesNeeded );
}


LogEntryParser_pythonSocket_Receiver::~LogEntryParser_pythonSocket_Receiver ()
{
  qDebug() << "receiver destroyed";
}

void LogEntryParser_pythonSocket_Receiver::newDataAvailable ()
{
  sizePayload_t sizeToReadNext = 0;

  std::list<TSharedLogEntry> entries;

  while (m_socket->bytesAvailable())
  {
    // read outstanding data
    readDataToBuffer();

    if (m_bytesNeeded == 0)
    {
      // Interpret data
      if (m_stateReadSize)
      {
        // This is the first length of package field
        sizeToReadNext = *reinterpret_cast<const sizePayload_t *>(&m_buffer[0]);
        boost::endian::big_to_native_inplace(sizeToReadNext);
        // Ignore message packets with 0 byte length. This is used for tunnel probing.
        if (sizeToReadNext == 0)
        {
          sizeToReadNext = sizeof(sizePayload_t);
        }
        else
        {
          m_stateReadSize = false;
        }
      }
      else
      {
        // This is the payload data
        entries.push_back(bufferToEntry());

        m_stateReadSize = true;
        sizeToReadNext = sizeof(sizePayload_t);
      }

      // We will shutdown if we receive more then 1M of data.
      // This is a prevention from memory errors due to wrong data.
      if (sizeToReadNext > 1024 * 1024)
      {
        // Purge entries before we will shutdown the connection.
        if (!entries.empty())
          Q_EMIT newEntry(entries);

        shutdown();
        return;
      }

      m_buffer.clear();
      m_buffer.resize( sizeToReadNext );
      m_bytesNeeded = sizeToReadNext;
    }
  }

  if (!entries.empty())
    Q_EMIT newEntry(entries);
}

namespace py = boost::python;

namespace
{
  std::string parse_python_exception ()
  {
    PyObject *type_ptr = nullptr, *value_ptr = nullptr, *traceback_ptr = nullptr;
    PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);

    // Fallback error
    std::string ret("Untyped Python error");
    if (type_ptr != nullptr)
    {
      py::handle<> h_type(type_ptr);
      py::str type_pstr(h_type);
      py::extract<std::string> e_type_pstr(type_pstr);
      if (e_type_pstr.check())
        ret = e_type_pstr();
      else
        ret = "Unknown exception type";
    }

    if (value_ptr != nullptr)
    {
      py::handle<> h_val(value_ptr);
      py::str a(h_val);
      py::extract<std::string> returned(a);
      if (returned.check())
        ret += ": " + returned();
      else
        ret += std::string(": Unparsable Python error");
    }

    if (traceback_ptr != nullptr)
    {
      py::handle<> h_tb(traceback_ptr);
      py::object tb(py::import("traceback"));
      py::object fmt_tb(tb.attr("format_tb"));
      py::object tb_list(fmt_tb(h_tb));
      py::object tb_str(py::str("\n").join(tb_list));
      py::extract<std::string> returned(tb_str);
      if (returned.check())
        ret += ": " + returned();
      else
        ret += std::string(": Python traceback not available");
    }
    return ret;
  }
}



TSharedLogEntry LogEntryParser_pythonSocket_Receiver::bufferToEntry ()
{
  TSharedLogEntry entry = m_server->myFactory->getNewLogEntry();

  try
  {
    if (m_pickleImport.is_none())
    {
      Py_Initialize();
      m_pickleImport = py::import("pickle");
    }
    py::object memoryView(py::handle<>(PyMemoryView_FromMemory(&m_buffer[0], m_buffer.size(), PyBUF_READ)));
    py::object rv  = m_pickleImport.attr("loads")(memoryView);
    py::dict logData = py::extract<py::dict>(rv);

//    {'name': 'root', 'msg': 'Root logging info test.', 'args': None, 'levelname': 'INFO',
//      'levelno': 20, 'pathname': './logProducer.py', 'filename': 'logProducer.py',
//      'module': 'logProducer', 'exc_info': None, 'exc_text': None, 'stack_info': None,
//      'lineno': 12, 'funcName': '<module>', 'created': 1555764493.4838123, 'msecs': 483.8123321533203,
//      'relativeCreated': 4.420280456542969, 'thread': 139974805075776, 'threadName': 'MainThread',
//      'processName': 'MainProcess', 'process': 13562}

    boost::python::extract<double> created( logData["created"]);
    boost::python::extract<double> msecs( logData["msecs"]);
    if (created.check() && msecs.check())
    {
      QDateTime timestamp(
          QDateTime::fromMSecsSinceEpoch(
              qint64(floor(created())) * 1000
                  + ((qint64(round(msecs()))))));
      entry->setAttribute(QVariant(timestamp),1);
    }

    boost::python::extract<std::string> name( logData["name"]);
    if (name.check())
      entry->setAttribute(QVariant(QString::fromStdString(name())),4);

    boost::python::extract<std::string> msg( logData["msg"]);
    if (msg.check())
      entry->setAttribute(QVariant(QString::fromStdString(msg())),2);

    boost::python::extract<std::string> levelname( logData["levelname"]);
    if (levelname.check())
      entry->setAttribute(QVariant(QString::fromStdString(levelname)),3);

    boost::python::extract<std::string> filename( logData["filename"]);
    if (filename.check())
      entry->setAttribute(QVariant(QString::fromStdString(filename())),6);

    boost::python::extract<std::string> funcName( logData["funcName"]);
    if (funcName.check())
      entry->setAttribute(QVariant(QString::fromStdString(funcName())),11);

    boost::python::extract<std::string> pathname( logData["pathname"]);
    boost::python::extract<uint64_t> lineno( logData["lineno"]);
    if (pathname.check() && lineno.check())
    {
      QString fsource = QString::fromStdString(pathname()) + ":" + QString::number(lineno());
      entry->setAttribute(QVariant(fsource),5);
    }

    boost::python::extract<uint64_t> thread( logData["thread"]);
    if (thread.check())
      entry->setAttribute(QVariant(qulonglong(thread())),7);

    boost::python::extract<std::string> threadName( logData["threadName"]);
    if (threadName.check())
      entry->setAttribute(QVariant(QString::fromStdString(threadName())),8);

    boost::python::extract<int> process( logData["process"]);
    if (process.check())
      entry->setAttribute(QVariant(qulonglong(process())),9);

    boost::python::extract<std::string> processName( logData["processName"]);
    if (processName.check())
      entry->setAttribute(QVariant(QString::fromStdString(processName())),10);

  }
  catch(boost::python::error_already_set const &){
      // Parse and output the exception
      std::string perror_str = parse_python_exception();
      qDebug() << "Error in Python: " << QString::fromStdString(perror_str);
      if (m_raiseErrors)
      {
        Q_EMIT error( QString("Got error from python (this will be reported only once):") + QString::fromStdString(perror_str) );
        m_raiseErrors = false;
      }
  }

  return entry;
}

void LogEntryParser_pythonSocket_Receiver::readDataToBuffer ()
{
  const qint64 toRead = std::min<qint64>(m_socket->bytesAvailable(),
                                         m_bytesNeeded);
  size_t position = m_buffer.size() - m_bytesNeeded;

  qint64 res = m_socket->read(&m_buffer[position], toRead);
  m_bytesNeeded -= res;
}

void LogEntryParser_pythonSocket_Receiver::shutdown ()
{
  qDebug() << "shutdown called on receiver!";
  if (m_socket->isValid() && m_socket->isOpen())
  {
    m_socket->close();
  }
  deleteLater();
}

