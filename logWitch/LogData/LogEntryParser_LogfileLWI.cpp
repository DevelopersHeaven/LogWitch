/*
 * LogEntryParser_LogfileLWI.cpp
 *
 *  Created on: Dec 18, 2011
 *      Author: sven
 */

#include "LogEntryParser_LogfileLWI.h"

#include <QRegExp>
#include <QtCore/QtCore>

#include "LogData/LogEntryAttributeNames.h"
#include "LogData/LogEntryFactory.h"
#include "LogData/LogEntryParserModelConfiguration.h"

LogEntryParser_LogfileLWI::LogEntryParser_LogfileLWI( const QString &filename)
: m_abort(false )
, m_logfile( filename )
, m_logfileStreamReady( false )
, m_logEntryNumber( 0 )
{
}

LogEntryParser_LogfileLWI::~LogEntryParser_LogfileLWI()
{
  m_abort = true;
  wait();
}

void LogEntryParser_LogfileLWI::startEmiting()
{
  if (!isRunning() && !m_abort)
    start(LowPriority);
}

std::shared_ptr<LogEntryParserModelConfiguration> LogEntryParser_LogfileLWI::getParserModelConfiguration() const
{
  return m_myModelConfig;
}

bool LogEntryParser_LogfileLWI::initParser()
{
  if (!m_logfile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    m_initError = tr("Unable to open text file!");
    return false;
  }

  m_logfileStream.setDevice(&m_logfile);
  m_logfileStreamReady = true;

  const QString LWIIdString("%%LWI_DESC=");

  QString firstLine = m_logfileStream.readLine();
  if (!firstLine.startsWith(LWIIdString))
  {
    m_initError = tr("Unable to detect LogWitchs magic code in line: '%1'").arg(
        firstLine);
    return false;
  }

  firstLine.remove(0, LWIIdString.length());
  QStringList descriptions = firstLine.split(" - ");

  qDebug() << " File identification string is: " << firstLine;

  m_factory.reset(new LogEntryFactory);

  m_order.resize(descriptions.size());

  LogEntryAttributeNames names;
  {
    // This section parses the description and replays the correct ordering and format strings.
    std::vector<AttributeConfiguration> configs;
    configs.reserve(descriptions.size());

    QRegExp expr("^([^:]*):(.*)$");
    QRegExp exprNumber("^(.*)\\(([0-9]*)\\)$");
    bool invalidOrdering = false;
    size_t i = 0;

    for (QStringList::iterator it = descriptions.begin();
        it != descriptions.end(); ++it, ++i)
    {
      QString name, extension;
      int orderId = -1;

      if (expr.exactMatch(*it))
      {
        qDebug() << " Field string matched extended: name " << expr.cap(1)
            << " extension: " << expr.cap(2);
        name = expr.cap(1);
        extension = expr.cap(2);
      }
      else
      {
        qDebug() << " Field string: name " << *it;
        name = *it;
      }

      if (exprNumber.exactMatch(name))
      {
        qDebug() << " Ordering match: name:" << name << " number: "
            << exprNumber.cap(2);
        name = exprNumber.cap(1);

        bool ok = false;
        orderId = exprNumber.cap(2).toInt(&ok);
        if (!ok)
          orderId = -1;
      }

      AttributeConfiguration cfg(names.getConfiguration(name));
      if (!extension.isEmpty())
        cfg.attributeFactory->setImportExportDescription(extension);
      configs.push_back(cfg);

      if (orderId < 0)
      {
        qDebug() << " Invalid ordering detected!";
        invalidOrdering = true;
      }
      else
        m_order[i] = orderId;
    }

    do
    {
      if (invalidOrdering)
      {
        for (i = 0; i < m_order.size(); ++i)
          m_order[i] = i;
        invalidOrdering = false;
      }

      for (i = 0; i < m_order.size(); ++i)
      {
        std::vector<int>::iterator it;
        it = find(m_order.begin(), m_order.end(), i);
        if (it == m_order.end())
        {
          qDebug() << " Invalid ordering detected (Step2)!";
          invalidOrdering = true;
          break;
        }

        m_factory->addField(configs[it - m_order.begin()]);
      }
    } while (invalidOrdering);
  }

  const QString LWIContextID("%%LWI_CFGContext=");
  QString modelContext("LogfileLWI");
  QString line = m_logfileStream.readLine();
  if (line.startsWith(LWIContextID))
  {
    line.remove(0, LWIContextID.length());
    modelContext = line;
  }
  else
  {
    m_stashedLine = line;
  }

  m_factory->disallowAddingFields();
  qDebug() << "Using Model context: " << modelContext;
  m_myModelConfig = std::shared_ptr<LogEntryParserModelConfiguration>(
      new LogEntryParserModelConfiguration(modelContext, m_factory));
  // TODO: Splitstrings?
  //m_myModelConfig->setHierarchySplitString( 4, "\\.");

  for (int i = 0; i < m_factory->getNumberOfFields(); ++i)
  {
    const AttributeConfiguration &cfg = m_factory->getFieldConfiguration(i);
    m_myModelConfig->setFieldWidthHint(i, cfg.defaultCellWidth, true);
  }

  return true;
}

void LogEntryParser_LogfileLWI::run()
{
  TSharedNewLogEntryMessage newEntryMessage(new NewLogEntryMessage);
  qDebug() << "Start parsing logfile (LWI) with run()";

  while (!m_abort)
  {
    TSharedLogEntry entry(getNextLogEntry());

    if (entry)
      newEntryMessage->entries.push_back(entry);
    else
      m_abort = true;
  }

  qDebug() << "We got " << newEntryMessage->entries.size()
      << " entries from logfile.";
  emit newEntry(newEntryMessage);
  emit finished();
}

TSharedLogEntry LogEntryParser_LogfileLWI::logEntryFromString(
    const QString &str)
{
  TSharedLogEntry entry = m_factory->getNewLogEntry();

  return entry;
}

TSharedLogEntry LogEntryParser_LogfileLWI::getNextLogEntry()
{
  TSharedLogEntry entryReturn;

  while (m_logfileStreamReady
      && (!m_logfileStream.atEnd() || !m_stashedLine.isEmpty()) && !entryReturn)
  {
    QString logEntryString(m_stashedLine);
    m_stashedLine.clear();

    // Step 1
    // Read one entry. One entry is at the end if on the next line is no
    // ' ' space at beginning.
    {
      bool lineComplete = false;
      while (!m_logfileStream.atEnd() && !lineComplete)
      {
        if (m_stashedLine.isEmpty())
          m_stashedLine = m_logfileStream.readLine();

        if (logEntryString.isEmpty())
        {
          logEntryString = m_stashedLine;
          m_stashedLine.clear();
        }
        else
        {
          if (m_stashedLine.isEmpty() || m_stashedLine.at(0) != ' ')
            lineComplete = true;
          else
          {
            logEntryString.append(m_stashedLine);
            m_stashedLine.clear();
          }
        }
      }

      // qDebug() << "Working on line: " << logEntryString;
    }

    // Step 2
    // Analyze and split string to fields.
    {
      QString splitSting(" - ");
      QStringList parts = logEntryString.split(splitSting);
      if (parts.size() > m_factory->getNumberOfFields())
      {
        QStringList::iterator itBegin = parts.begin()
            + m_factory->getNumberOfFields() - 1;
        QStringList message;
        for (QStringList::iterator it = itBegin; it != parts.end(); ++it)
          message.push_back(*it);
        parts.erase(itBegin, parts.end());
        parts.push_back(message.join(splitSting));
      }

      // Check if entry is broken, we will skip it!
      // TODO: Add error message?
      if (parts.size() == m_factory->getNumberOfFields())
      {
        TSharedLogEntry entry = m_factory->getNewLogEntry();
        for (int i = 0; i < m_factory->getNumberOfFields(); ++i)
        {
          entry->setAttribute(
              (*m_factory->getFieldConfiguration(m_order[i]).attributeFactory)(
                  parts.at(i)), m_order[i]);
        }

        entryReturn = entry;
      }
      else
      {
        qDebug() << " >> Unable to parse LogEntry: We found only: "
            << parts.size() << " entries, but we need: "
            << m_factory->getNumberOfFields();
      }
    }
  }

  // qDebug() << " >> LogEntry parsed and returns: "<< entryReturn.get()<<"  << ";
  return entryReturn;
}
