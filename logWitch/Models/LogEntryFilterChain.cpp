/*
 * LogEntryFilterChain.cpp
 *
 *  Created on: May 19, 2011
 *      Author: sven
 */

#include "LogEntryFilterChain.h"

LogEntryFilterChain::LogEntryFilterChain()
	: m_changeCounter( 0 )
{
}

LogEntryFilterChain::~LogEntryFilterChain()
{
}

void LogEntryFilterChain::addFilter( boost::shared_ptr<LogEntryFilter> flt )
{
	flt->setParent( this );

	m_filterChain.push_back( flt );
}

void LogEntryFilterChain::removeFilter( boost::shared_ptr<LogEntryFilter>  flt )
{
	flt->setParent( NULL );

	TFilterChain::iterator it = m_filterChain.begin();
	for( ; it != m_filterChain.end(); ++it )
	{
		if( *it == flt )
		{
			m_filterChain.erase( it );
			break;
		}
	}
}

bool LogEntryFilterChain::filterEntry( TconstSharedLogEntry entry ) const
{
	TFilterChain::const_iterator it = m_filterChain.begin();
	for( ; it != m_filterChain.end(); ++it )
	{
		if( !(*it)->filterEntry( entry ) )
			return false;
	}

	return true;
}

void LogEntryFilterChain::LogEntryFilterChain::startChange()
{
	m_changeCounter++;
}

void LogEntryFilterChain::LogEntryFilterChain::endChange()
{
	m_changeCounter--;
	if( m_parent == NULL && 0 == m_changeCounter )
	{
		emit filterUpdateFinished();
	}
}

