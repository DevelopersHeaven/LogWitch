/*
 * LogEntryStorerForTesting.h
 *
 *  Created on: 12 Mar 2016
 *      Author: sven
 */

#ifndef LOGWITCH_LOGDATA_TEST_LOGENTRYSTORERFORTESTING_H_
#define LOGWITCH_LOGDATA_TEST_LOGENTRYSTORERFORTESTING_H_

#include <QtCore/qobject.h>

#include <list>

#include "LogData/NewLogEntryMessage.h"

class LogEntryParser;

class LogEntryStorerForTesting: public QObject
{
  Q_OBJECT
public:
  LogEntryStorerForTesting( LogEntryParser* parser );

  virtual ~LogEntryStorerForTesting();

  void start();

public slots:
  void onNewEntry( TconstSharedNewLogEntryMessage );

  void onSignalError( QString error );

  void onFinished();

  void runParser();

public:
  std::list<TconstSharedNewLogEntryMessage> m_entryMessages;

  std::list<TSharedLogEntry> m_entries;

  std::list<QString> m_errors;

  bool m_finished;

  LogEntryParser* m_parser;

  QCoreApplication* m_app;
};

#endif /* LOGWITCH_LOGDATA_TEST_LOGENTRYSTORERFORTESTING_H_ */
