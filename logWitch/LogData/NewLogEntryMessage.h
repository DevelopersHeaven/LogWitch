/*
 * NewLogEntryMessage.h
 *
 *  Created on: Dec 2, 2011
 *      Author: sven
 */
#ifndef NEWLOGENTRYMESSAGE_H_
#define NEWLOGENTRYMESSAGE_H_

#include <list>

#include <QtCore>

#include "LogData/LogEntry.h"

class NewLogEntryMessage
: public QObject
{
    Q_OBJECT
public:
    NewLogEntryMessage();

    std::list<TSharedLogEntry> entries;
};

typedef std::shared_ptr<NewLogEntryMessage> TSharedNewLogEntryMessage;
typedef std::shared_ptr<const NewLogEntryMessage> TconstSharedNewLogEntryMessage;


#endif /* NEWLOGENTRYMESSAGE_H_ */
