/*
 * EntryToTextFormaterLog4cplus.h
 *
 *  Created on: Jun 2, 2011
 *      Author: sven
 */

#ifndef ENTRYTOTEXTFORMATERLOG4CPLUS_H_
#define ENTRYTOTEXTFORMATERLOG4CPLUS_H_

#include "LogData/EntryToTextFormater.h"

namespace logwitch { namespace plugins { namespace log4cplus {

/**
 * Formats the entry stuff to the format we want. This is a special factor for
 * using with the Log4cplus framework.
 */
class EntryToTextFormaterLog4cplus
    : public EntryToTextFormater
{
public:
    QString formatEntry(TconstSharedLogEntry entry) const override;
};

}}}

#endif /* ENTRYTOTEXTFORMATERLOG4CPLUS_H_ */
