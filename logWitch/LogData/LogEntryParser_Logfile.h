/*
 * LogEntryParser_Logfile.h
 *
 *  Created on: May 13, 2011
 *      Author: sven
 */

#ifndef LOGENTRYPARSER_LOGFILE_H_
#define LOGENTRYPARSER_LOGFILE_H_

#include "LogEntryParser.h"

#include <QFile>
#include <QTextStream>
#include <QtCore/QtCore>
#include "LogEntryFactory.h"

class LogEntryOld;
class LogEntryParserModelConfiguration;
class ParserStreamGetter;

class LogEntryParser_Logfile
	: public QThread
	, public LogEntryParser
{
	Q_OBJECT
public:
	LogEntryParser_Logfile( std::shared_ptr<ParserStreamGetter> getter );

	~LogEntryParser_Logfile();

	void startEmiting();

	bool initParser();

	void run();

	virtual std::shared_ptr<LogEntryParserModelConfiguration> getParserModelConfiguration() const;

	QString getName() const;
signals:
	void newEntry( TconstSharedNewLogEntryMessage );

    void signalError( QString error );

  void finished();

private:
  TSharedNewLogEntryMessage getEntries();

	bool m_abort;

	std::shared_ptr<ParserStreamGetter> m_getter;

	std::shared_ptr<QTextStream> m_logfileStream;

	QString timeFormat;

	std::shared_ptr<LogEntryFactory> myFactory;

	std::shared_ptr<LogEntryParserModelConfiguration> m_myModelConfig;

	int m_logEntryNumber;

	class LogfileLine;
	class PreLogEntry;
	class WorkPackage;

	std::shared_ptr<LogfileLine> analyzeLine() const;
	TSharedLogEntry createLogEntry( PreLogEntry& pre );

};

#endif /* LOGENTRYPARSER_LOGFILE_H_ */
