/*
 * LogEntryFilter.h
 *
 *  Created on: May 18, 2011
 *      Author: sven
 */

#ifndef LOGENTRYFILTER_H_
#define LOGENTRYFILTER_H_
#include "LogData/LogEntry.h"

class LogEntryFilter {
public:
	LogEntryFilter() : m_parent(nullptr) {}

	virtual ~LogEntryFilter( );

	virtual void setParent( LogEntryFilter *parent ){ m_parent = parent; }

	virtual bool filterEntry( TconstSharedLogEntry entry ) const = 0;

	virtual void startChange() { if( m_parent ) m_parent->startChange(); };

	virtual void endChange() { if( m_parent ) m_parent->endChange(); };

protected:
	LogEntryFilter *m_parent;
};

#endif /* LOGENTRYFILTER_H_ */
