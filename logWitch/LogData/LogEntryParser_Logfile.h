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
#include <QThread>
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

	~LogEntryParser_Logfile() override;

	void startEmiting() override;

	bool initParser() override;

	void run() override;

	std::shared_ptr<LogEntryParserModelConfiguration> getParserModelConfiguration() const override;

	QString getName() const override;
signals:
	void newEntry(TconstSharedNewLogEntryMessage) override;

    void signalError(QString error) override;

  void finished() override;

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
