/*
 * LogEntryFilterChain.h
 *
 *  Created on: May 19, 2011
 *      Author: sven
 */

#ifndef LOGENTRYFILTERCHAIN_H_
#define LOGENTRYFILTERCHAIN_H_

#include "Models/LogEntryFilter.h"
#include <list>
#include <QtCore/QtCore>

class LogEntryFilterChain
	: public QObject
	, LogEntryFilter
{
	Q_OBJECT
public:
	LogEntryFilterChain();

	virtual ~LogEntryFilterChain();

	void addFilter( std::shared_ptr< LogEntryFilter> );

	void removeFilter( std::shared_ptr< LogEntryFilter> );

	virtual bool filterEntry( TconstSharedLogEntry entry ) const;

	virtual void startChange();

	virtual void endChange();

signals:
	void filterUpdateFinished();

private:
	typedef std::list< std::shared_ptr<  LogEntryFilter> > TFilterChain;
	TFilterChain m_filterChain;

	int m_changeCounter;
};

#endif /* LOGENTRYFILTERCHAIN_H_ */
