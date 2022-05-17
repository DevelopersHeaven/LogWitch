/*
 * LogEntryParser_Logfile.cpp
 *
 *  Created on: May 13, 2011
 *      Author: sven
 */

#include "LogEntryParser_Logfile.h"

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>

#include <QRegExp>
#include <QtCore/QtCore>
#include <QtConcurrent>

#include "LogEntryFactory.h"
#include "LogEntryAttributeNames.h"
#include "LogData/LogEntry.h"
#include "LogData/LogEntryFactory.h"
#include "LogData/LogEntryParserModelConfiguration.h"
#include "LogData/ParserStreamGetter.h"

LogEntryParser_Logfile::LogEntryParser_Logfile(  std::shared_ptr<ParserStreamGetter> getter  )
  : m_abort(false )
  , m_getter(getter)
  , lineMessageRegex( new QRegExp("^([\\d-]+\\s+[\\d\\,\\:]+)\\s+-\\s+((?:(?!\\s+-\\s+).)*)\\s+-\\s+((?:(?!\\s+-\\s+).)*)\\s+-\\s+(\\[(.*)\\]|((?!\\s+-\\s+).)*)\\s+-\\s+(.*)$") )
  , cellRegex( "\\s+-\\s+" )
  , timeFormat( "yyyy-MM-dd HH:mm:ss,zzz" )
  , myFactory( new LogEntryFactory )
  , m_logEntryNumber( 0 )
{
  lineMessageRegex->setMinimal(true);

  // Preparing attributes in factory
  LogEntryAttributeNames names;
  myFactory->addField(names.getConfiguration("number"));
  myFactory->addField(names.getConfiguration("timestamp"));
  myFactory->addField(names.getConfiguration("message"));

  myFactory->addField(names.getConfiguration("level"));
  myFactory->addField(names.getConfiguration("logger"));
  myFactory->addField(names.getConfiguration("fsource"));
  myFactory->disallowAddingFields();

  m_myModelConfig = std::shared_ptr<LogEntryParserModelConfiguration>(
      new LogEntryParserModelConfiguration("Logfile", myFactory));
  m_myModelConfig->setHierarchySplitString(4, "\\.");
  m_myModelConfig->setHierarchySplitString(5, "/");

  for (int i = 0; i < myFactory->getNumberOfFields(); ++i)
  {
    m_myModelConfig->setFieldWidthHint(i,
        myFactory->getFieldConfiguration(i).defaultCellWidth, true);
  }

  const auto fieldOrderHint = boost::assign::list_of(0)(5)(1)(2)(3)(4);
  m_myModelConfig->setFieldOrderHint(
      QVector<int>(fieldOrderHint.begin(), fieldOrderHint.end()),
      true);
}

LogEntryParser_Logfile::~LogEntryParser_Logfile()
{
  m_abort = true;
  wait();
}

QString LogEntryParser_Logfile::getName() const
{
  return m_getter->getName();
}

void LogEntryParser_Logfile::startEmiting()
{
  if (!isRunning() && !m_abort)
    start(LowPriority);
}

std::shared_ptr<LogEntryParserModelConfiguration> LogEntryParser_Logfile::getParserModelConfiguration() const
{
  return m_myModelConfig;
}

bool LogEntryParser_Logfile::initParser()
{
  m_logfileStream = m_getter->getStream();

  if (!m_logfileStream)
  {
    m_initError = m_getter->getError();
    return false;
  }

  return true;
}

void LogEntryParser_Logfile::run()
{
  TSharedNewLogEntryMessage newEntryMessage;
  qDebug() << "Start parsing logfile with run()";

  QElapsedTimer myTimer;
  myTimer.start();
  newEntryMessage = getEntries();
  int nMilliseconds = myTimer.elapsed();

  qDebug() << "We got " << newEntryMessage->entries.size()
      << " entries from logfile within " << double(nMilliseconds)/1000 << " seconds";
  emit newEntry(newEntryMessage);
  emit finished();
}

class LogEntryParser_Logfile::LogfileLine
{
public:
  void setLine( QString line ){
    m_line = line;
  }

  bool match() {
    return (m_matched->indexIn(m_line) != -1);
  }

  QString m_line;
  std::shared_ptr<QRegExp> m_matched;
};

class LogEntryParser_Logfile::PreLogEntry
{
public:
  PreLogEntry(std::shared_ptr<LogfileLine> line): m_firstLine(line) {};
  /**
   * Contains the first line, which must have a valid match
   */
  std::shared_ptr<LogfileLine> m_firstLine;

  /**
   * Lines which have no match and are parts of this entry.
   */
  QStringList m_nextLines;
};

class LogEntryParser_Logfile::WorkPackage
{
public:
  typedef std::list< std::shared_ptr<LogfileLine> > LogLineList;
  void convert(){
    // Here we have to store the line we have the first match. If there is no
    // match, we can not use the line or it is a message from the previous block.
    LogLineList::iterator itLastNonProcessedLine = m_lines.begin();

    LogLineList::iterator it = m_lines.begin();

    std::shared_ptr<PreLogEntry> preEntry;
    if (!m_preLogEntries.empty()) {
      preEntry = m_preLogEntries.back();
      m_preLogEntries.pop_back();
    }

    for (; it != m_lines.end(); ++it)
    {
      if ((*it)->match())
      {
        if (preEntry) {
          m_preLogEntries.push_back( preEntry );
        }

        preEntry.reset(new PreLogEntry(*it) );
      }
      else
      {
        if (preEntry) {
          preEntry->m_nextLines << (*it)->m_line;
        }
        else {
          itLastNonProcessedLine = it;
          itLastNonProcessedLine++;
        }
      }
    }

    if (preEntry) {
      m_preLogEntries.push_back( preEntry );
      m_lines.erase( itLastNonProcessedLine, m_lines.end() );
    }

    // qDebug() << " Convertions done: " << id;
  }

  /**
   * Adds all stuff from the nextWorkPackage to this package. The nextWorkPackage
   * can be discarded afterwards.
   */
  void join( WorkPackage& nextWorkPackage )
  {
    Q_ASSERT(m_lines.empty());

    m_lines = nextWorkPackage.m_lines;
    convert();

    m_preLogEntries.insert( m_preLogEntries.end()
        , nextWorkPackage.m_preLogEntries.begin()
        , nextWorkPackage.m_preLogEntries.end() );
  }

  void runConvert()
  {
    m_future = QtConcurrent::run(this, &LogEntryParser_Logfile::WorkPackage::convert);
  }

  void waitForFinish()
  {
    m_future.waitForFinished();
  }

  int id;

  QFuture<void> m_future;

  LogLineList m_lines;

  std::list< std::shared_ptr<PreLogEntry> > m_preLogEntries;
};


TSharedLogEntry LogEntryParser_Logfile::createLogEntry( PreLogEntry& pre )
{
  QRegExp& lineMessageRegex = *pre.m_firstLine->m_matched;
  TSharedLogEntry entry = myFactory->getNewLogEntry();

  entry->setAttribute(QVariant(++m_logEntryNumber), 0);

  entry->setAttribute(
      QVariant(QDateTime::fromString(lineMessageRegex.cap(1), timeFormat)), 1);
  // File Source
  if (lineMessageRegex.cap(5).isEmpty())
    entry->setAttribute(QVariant(lineMessageRegex.cap(4)), 5);
  else
    entry->setAttribute(QVariant(lineMessageRegex.cap(5)), 5);

  QString message = lineMessageRegex.cap(7);
  if (!pre.m_nextLines.isEmpty())
    message += "\n" + pre.m_nextLines.join("\n");
  entry->setAttribute(QVariant(QString(message)), 2);

  // Severity
  entry->setAttribute(QVariant(lineMessageRegex.cap(2)), 3);
  // Component
  entry->setAttribute(QVariant(lineMessageRegex.cap(3)), 4);

  return entry;
}

TSharedNewLogEntryMessage LogEntryParser_Logfile::getEntries()
{
  TSharedNewLogEntryMessage entryReturn(new NewLogEntryMessage);

  if (m_logfileStream)
  {
    std::list< std::shared_ptr<LogfileLine> > lineProcessors;
    QElapsedTimer myTimer;
    myTimer.start();

    std::list< std::shared_ptr<WorkPackage> > workPackages;

    int currentWork = 0;
    int maxWork = 1000;
    while( !m_logfileStream->atEnd())
    {
      std::shared_ptr<LogfileLine> lineProcessor( new LogfileLine );
      lineProcessor->m_matched = std::shared_ptr<QRegExp>(new QRegExp(*lineMessageRegex) );
      lineProcessor->setLine( m_logfileStream->readLine() );
      lineProcessors.push_back(lineProcessor);
      currentWork++;

      if (m_logfileStream->atEnd() || currentWork >= maxWork )
      {
        std::shared_ptr<WorkPackage> work( new WorkPackage );
        work->m_lines.swap(lineProcessors);
        work->id = workPackages.size();
        workPackages.push_back( work );
        work->runConvert();
        currentWork = 0;
        // qDebug() << " Submitting package #" << workPackages.size();
      }
    }

//    {
//      int nMilliseconds = myTimer.elapsed();
//      qDebug() << "After reading completed: " << double(nMilliseconds)/1000 << " seconds";
//    }

    if (workPackages.empty())
      return entryReturn;

    std::shared_ptr<WorkPackage> firstWorkPackage = workPackages.front();
    workPackages.pop_front();
    firstWorkPackage->waitForFinish();

    if (!firstWorkPackage->m_lines.empty()) {
      qDebug() << " We have pending lines we are unable to parse, dropping";
      firstWorkPackage->m_lines.clear();
    }

    while( !workPackages.empty() )
    {
      std::shared_ptr<WorkPackage> currentWork = workPackages.front();
      workPackages.pop_front();
      currentWork->waitForFinish();
//      {
//        int nMilliseconds = myTimer.elapsed();
//        qDebug() << " Next package ("<<currentWork->id<<") arrived after: " << double(nMilliseconds)/1000 << " seconds";
//      }

      firstWorkPackage->join( *currentWork );

      // The last enry is not finished, so we will take it for later usage ...
      if (!firstWorkPackage->m_preLogEntries.empty()){
        std::shared_ptr<PreLogEntry> unfinishedPreEntry = firstWorkPackage->m_preLogEntries.back();
        firstWorkPackage->m_preLogEntries.pop_back();

        while (!firstWorkPackage->m_preLogEntries.empty()){
          std::shared_ptr<PreLogEntry> preEntry = firstWorkPackage->m_preLogEntries.front();
          firstWorkPackage->m_preLogEntries.pop_front();

          entryReturn->entries.push_back( createLogEntry(*preEntry) );
        }

        firstWorkPackage->m_preLogEntries.push_back(unfinishedPreEntry);
      }
    }

//    {
//      int nMilliseconds = myTimer.elapsed();
//      qDebug() << "After matching: " << double(nMilliseconds)/1000 << " seconds";
//    }

    BOOST_FOREACH( boost::shared_ptr<PreLogEntry> preEntry, firstWorkPackage->m_preLogEntries )
    {
      entryReturn->entries.push_back( createLogEntry(*preEntry) );
    }

//    {
//      int nMilliseconds = myTimer.elapsed();
//      qDebug() << "After processing: " << double(nMilliseconds)/1000 << " seconds";
//    }
  }
  return entryReturn;
}

