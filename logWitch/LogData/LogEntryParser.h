/*
 * LogEntryParser.h
 *
 *  Created on: May 13, 2011
 *      Author: sven
 */

#ifndef LOGENTRYPARSER_H_
#define LOGENTRYPARSER_H_

#include <QtGui>
#include <QtCore/QObject>

#include "LogData/LogEntry.h"
#include "NewLogEntryMessage.h"

class LogEntryParserModelConfiguration;

/**
 * This class parses a line or more lines from a log file and passes the found
 * elements to the LogEntryFactory for generating a new LogEntry.
 */
class LogEntryParser
{
public:
	virtual ~LogEntryParser(){}

	/**
	 * This method returns a name which can be used to identify the origin, for eg. Server, logfilename, etc.
	 */
	virtual QString getName() const { return QString("Untitled"); }

	/**
	 * Gets a human readbale string with an error description if initParser
	 * fails.
	 */
	virtual QString getInitError() const { return m_initError; }

	/**
	 * Initializes the parser and checks if the parser is runable.
	 * After calling this, a valid parserModelConfiguration is available
	 * if init returns true.
	 *
	 * @return true if successful otherwise false.
	 */
	virtual bool initParser() = 0;

	/**
	 * This starts the parser after you have connected to the signals.
	 */
	virtual void startEmiting() = 0;

	/**
	 * This returns the parserModelConfiguration. This configuration
	 * will be available and unchanged after calling initParser till
	 * destruction.
	 */
	virtual std::shared_ptr<LogEntryParserModelConfiguration> getParserModelConfiguration() const = 0;

	// This block is the later called signals methods.
protected:
	virtual void newEntry( TconstSharedNewLogEntryMessage ) = 0;

	virtual void signalError( QString error ) = 0;

	virtual void finished() = 0;

	QString m_initError;
};

class LogFileParser
{
public:
    virtual ~LogFileParser() { };

    /**
     * Returns true if the parser seems to be working (parsers should try some loglines
     * if they can perse them and if this is possible, the return true here.)
     */
    virtual bool isParseable() const = 0;

};

#endif /* LOGENTRYPARSER_H_ */
