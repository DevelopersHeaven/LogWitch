/*
 * LogEntryFilterChain.cpp
 *
 *  Created on: May 19, 2011
 *      Author: sven
 */

#include "LogEntryFilterChain.h"

void LogEntryFilterChain::addFilter( std::shared_ptr<LogEntryFilter> flt )
{
	flt->setParent( this );

	m_filterChain.push_back( flt );
}

void LogEntryFilterChain::removeFilter( std::shared_ptr<LogEntryFilter>  flt )
{
	flt->setParent(nullptr);

	auto it = m_filterChain.begin();
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
	auto it = m_filterChain.begin();
	for( ; it != m_filterChain.end(); ++it )
	{
		if( !(*it)->filterEntry( entry ) )
			return false;
	}

	return true;
}

void LogEntryFilterChain::startChange()
{
	m_changeCounter++;
}

void LogEntryFilterChain::endChange()
{
	m_changeCounter--;
	if (m_parent == nullptr && 0 == m_changeCounter)
	{
		emit filterUpdateFinished();
	}
}

