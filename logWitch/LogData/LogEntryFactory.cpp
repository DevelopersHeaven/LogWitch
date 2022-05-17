/*
 * LogEntryAttributes.cpp
 *
 *  Created on: May 14, 2011
 *      Author: sven
 */

#include "LogEntryFactory.h"

#include <QtCore>

#include "LFAAssert.h"
#include "LogData/ObjectCache.hxx"
#include "LogData/LogEntry.h"


LogEntryFactory::LogEntryFactory()
	: m_disallowAddingFields( false )
{

}

LogEntryFactory::~LogEntryFactory()
{

}

std::shared_ptr<LogEntry> LogEntryFactory::getNewLogEntry()
{
	std::shared_ptr<LogEntry> attr( new LogEntry(this, m_defaultLine ) );

	return attr;
}

void LogEntryFactory::disallowAddingFields()
{
	m_disallowAddingFields = true;
}

void LogEntryFactory::addField( const AttributeConfiguration cfg )
{
	LFA_ASSERT( !m_disallowAddingFields, "Adding fields is not allowed!" );
	m_fieldDescriptions.push_back( cfg );

	if( cfg.caching )
	    m_fieldCaches.push_back( std::shared_ptr< ObjectCache<ObjectCacheQStringSignaller> >( new ObjectCache<ObjectCacheQStringSignaller> ) );
	else
	    m_fieldCaches.push_back( std::shared_ptr< ObjectPasser<ObjectCacheQStringSignaller> >( new ObjectPasser<ObjectCacheQStringSignaller> ) );

	m_defaultLine.push_back( QVariant::fromValue( m_fieldCaches.back()->getObject( std::shared_ptr<QString>(new QString("")) ) ) );
}

int LogEntryFactory::getNumberOfFields( ) const
{
	LFA_ASSERT_D( m_disallowAddingFields, "Getting numbers of fields only allowed if all fields are added!" );
	return m_fieldDescriptions.size();
}

const QString& LogEntryFactory::getDescLong( int idx ) const
{
	LFA_ASSERT_D( m_disallowAddingFields, "Getting descriptions of fields only allowed if all fields are added!" );
	return m_fieldDescriptions[idx].names.second;
}

const QString& LogEntryFactory::getDescShort( int idx ) const
{
	LFA_ASSERT_D( m_disallowAddingFields, "Getting descriptions of fields only allowed if all fields are added!" );
	return m_fieldDescriptions[idx].names.first;
}

const AttributeConfiguration &LogEntryFactory::getFieldConfiguration( int idx ) const
{
    LFA_ASSERT_D( m_disallowAddingFields, "Getting descriptions of fields only allowed if all fields are added!" );
    return m_fieldDescriptions[idx];
}

QString LogEntryFactory::getDescShortAsLongSring() const
{
    QStringList stringList;

    std::vector< AttributeConfiguration >::const_iterator it;

    for( it = m_fieldDescriptions.begin(); it != m_fieldDescriptions.end(); ++it )
    {
        stringList << it->names.first;
    }

    return stringList.join("-");
}


GetObjectIF<ObjectCacheQStringSignaller>& LogEntryFactory::getCache( int idx )
{
	LFA_ASSERT_D( m_disallowAddingFields, "Getting caches of fields only allowed if all fields are added!" );
	return *(m_fieldCaches[idx]);
}

const GetObjectIF<ObjectCacheQStringSignaller>& LogEntryFactory::getCache( int idx ) const
{
	LFA_ASSERT_D( m_disallowAddingFields, "Getting caches of fields only allowed if all fields are added!" );
	return *(m_fieldCaches[idx]);
}
