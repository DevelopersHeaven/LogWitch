/*
 * LogEntryStorerForTesting.cpp
 *
 *  Created on: 12 Mar 2016
 *      Author: sven
 */

#include <LogData/test/LogEntryStorerForTesting.h>

#include "LogData/LogEntryParser.h"

LogEntryStorerForTesting::LogEntryStorerForTesting( LogEntryParser* parser )
: m_finished (false)
, m_parser(parser)
, m_app(NULL)
{
  QObject* parserQO = dynamic_cast<QObject*>(parser);

  QObject::connect(parserQO, SIGNAL(newEntry(TconstSharedNewLogEntryMessage)), this, SLOT(onNewEntry(TconstSharedNewLogEntryMessage)));
  QObject::connect(parserQO, SIGNAL(signalError(QString)), this, SLOT(onSignalError(QString)));
  QObject::connect(parserQO, SIGNAL(finished()), this, SLOT(onFinished()));
}

void LogEntryStorerForTesting::start()
{
  int argc =0;
  char * argv[] = {0};
  QCoreApplication app(argc, argv);
  m_app = &app;

  // Ensure we will not test forever in error cases
  QTimer::singleShot(20000, &app, SLOT(quit()) );
  QTimer::singleShot(0, this, SLOT(runParser()) );
  app.exec();

  m_app = NULL;
}

void LogEntryStorerForTesting::runParser()
{
  if (!m_parser->initParser())
    m_app->quit();

  m_parser->startEmiting();
}

void LogEntryStorerForTesting::onNewEntry( TconstSharedNewLogEntryMessage msg )
{
  m_entryMessages.push_back(msg);
  m_entries.insert( m_entries.end(), msg->entries.begin(), msg->entries.end());
}

void LogEntryStorerForTesting::onSignalError( QString error )
{
  m_errors.push_back( error );
}

void LogEntryStorerForTesting::onFinished()
{
  m_finished = true;
  if (m_app)
    m_app->quit();
}
