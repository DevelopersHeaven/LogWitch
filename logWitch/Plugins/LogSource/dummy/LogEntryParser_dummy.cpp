/*
 * LogEntryParser_dummy.cpp
 *
 *  Created on: May 14, 2011
 *      Author: sven
 */

#include "LogEntryParser_dummy.h"

#include <boost/assign/list_of.hpp>

#include "LogData/LogEntry.h"
#include "LogData/LogEntryAttributeNames.h"
#include "LogData/LogEntryFactory.h"

using namespace logwitch::plugins::dummy;

LogEntryParser_dummy::LogEntryParser_dummy ()
: m_entries(0)
, m_abort(false)
, m_count(12)
, myFactory(new LogEntryFactory)
{
  // Preparing attributes factory
  LogEntryAttributeNames names;
  myFactory->addField(names.getConfiguration("number"));
  myFactory->addField(names.getConfiguration("timestamp"));
  myFactory->addField(names.getConfiguration("message"));

  myFactory->addField(names.getConfiguration("level"));
  myFactory->addField(names.getConfiguration("logger"));
  myFactory->disallowAddingFields();

  m_myModelConfig = boost::shared_ptr<LogEntryParserModelConfiguration>(
      new LogEntryParserModelConfiguration("DummyLogger", myFactory));
  m_myModelConfig->setHierarchySplitString(4, "\\.");

  for (int i = 0; i < myFactory->getNumberOfFields(); ++i)
  {
    const AttributeConfiguration &cfg = myFactory->getFieldConfiguration(i);
    m_myModelConfig->setFieldWidthHint(i, cfg.defaultCellWidth, true);
  }

  const auto fieldOrderHint = boost::assign::list_of(0)(4)(1)(2)(3);
  m_myModelConfig->setFieldOrderHint(
      QVector<int>(fieldOrderHint.begin(), fieldOrderHint.end()), true);
}

LogEntryParser_dummy::~LogEntryParser_dummy ()
{
  m_abort = true;
  wait();
}

void LogEntryParser_dummy::addEntries (int count)
{
  m_count = count;
}

void LogEntryParser_dummy::startEmiting ()
{
  if (!isRunning() && !m_abort)
    start(LowPriority);
}

void LogEntryParser_dummy::run ()
{
  forever
  {
    for (int i = 0; i < 4; i++)
    {
      usleep(250000);

      if (m_count)
        break;

      if (m_abort)
        return;
    }

    TSharedNewLogEntryMessage newEntryMessage(new NewLogEntryMessage);

    while (m_count)
    {
      TSharedLogEntry entry(getNextLogEntry());

      if (entry)
        newEntryMessage->entries.push_back(entry);

      m_count--;
      if (m_count < 0)
        m_count = 0;
    }

    TSharedLogEntry entry(getNextLogEntry());

    if (entry)
      newEntryMessage->entries.push_back(entry);

    if (!newEntryMessage->entries.empty())
      emit newEntry(newEntryMessage);
    else
      m_abort = true;
  }

  emit finished();
}

bool LogEntryParser_dummy::initParser ()
{
  static int inits = 0;

  if ((++inits % 3) == 0)
  {
    m_initError = tr("Parser initialization failed (just a test)");
    return false;

  }
  return true;
}

TSharedLogEntry LogEntryParser_dummy::getNextLogEntry ()
{
  TSharedLogEntry entry;
  //if( m_entries++ < 100000 )
  {
    m_entries++;

    if ((m_entries % 100) == 20 && m_emitError)
    {
      qDebug() << "emmiting error!";
      emit signalError(QString("This is a generated error from dummy parser!"));
    }

    entry = myFactory->getNewLogEntry();

    entry->setAttribute(QVariant(m_entries), 0);
    entry->setAttribute(QVariant(QDateTime::currentDateTime()), 1);
    QString message(QString("Message #").append(QString("%1").arg(m_entries)));
    entry->setAttribute(QVariant(message), 2);

    entry->setAttribute(QVariant(QString("DEBUG")), 3);
    if ((m_entries % 20) == 0)
      entry->setAttribute(QVariant(QString("WARN")), 3);
    if ((m_entries % 100) == 0)
      entry->setAttribute(QVariant(QString("ERROR")), 3);

    entry->setAttribute(QVariant(QString("Car.Engine")), 4);
    if ((m_entries % 3) == 0)
      entry->setAttribute(QVariant(QString("Car.Fuel")), 4);
    if ((m_entries % 10) == 0)
      entry->setAttribute(QVariant(QString("TestApp")), 4);
    if ((m_entries % 15) == 0)
      entry->setAttribute(QVariant(QString("Car.Tire")), 4);
    if ((m_entries % 20) == 0)
      entry->setAttribute(QVariant(QString("Car.Tire.right")), 4);
    if ((m_entries % 21) == 0)
      entry->setAttribute(QVariant(QString("Car.Tire.Left")), 4);
    if ((m_entries % 25) == 0)
      entry->setAttribute(QVariant(QString("Car.Crash")), 4);
    if ((m_entries % 30) == 0)
      entry->setAttribute(QVariant(QString("Car")), 4);

  }

  return entry;
}

boost::shared_ptr<LogEntryParserModelConfiguration> LogEntryParser_dummy::getParserModelConfiguration () const
{
  return m_myModelConfig;
}
