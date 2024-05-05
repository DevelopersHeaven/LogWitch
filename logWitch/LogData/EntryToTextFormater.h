/*
 * EntryToTextFormater.h
 *
 *  Created on: May 17, 2011
 *      Author: sven
 */

#ifndef ENTRYTOTEXTFORMATER_H_
#define ENTRYTOTEXTFORMATER_H_

#include "LogData/LogEntry.h"

/**
 * The interface is responsible for formatting the entry to a
 * readable text.
 */
class EntryToTextFormater
{
public:
	virtual ~EntryToTextFormater() = default;

	/**
	 * Formats the entry to a human readable html-text.
	 * @param entry Entry to format.
	 * @return Returns a html-text to display this entry.
	 */
	virtual QString formatEntry( TconstSharedLogEntry entry ) const = 0;
};

#endif /* ENTRYTOTEXTFORMATER_H_ */
