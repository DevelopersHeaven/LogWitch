/*
 * FieldDescriptor.cpp
 *
 *  Created on: Jun 5, 2011
 *      Author: sven
 */

#include "ActionRules/ValueGetterLogEntry.h"

#include <utility>


#include "LogData/LogEntryFactory.h"
#include "LogData/LogEntry.h"
#include "LogData/LogEntryParserModelConfiguration.h"

ValueGetterLogEntry::ValueGetterLogEntry( )
    : m_name( )
    , m_configuration( )
    , m_fieldId( -1 )
{

}

ValueGetterLogEntry::ValueGetterLogEntry(TSharedConstLogEntryParserModelConfiguration configuration )
    : m_name( )
    , m_configuration(std::move( configuration ))
    , m_fieldId( -1 )
{

}

ValueGetterLogEntry::ValueGetterLogEntry(QString name, TSharedConstLogEntryParserModelConfiguration configuration )
    : m_name(std::move( name ))
    , m_configuration(std::move( configuration ))
    , m_fieldId( -1 )
{
    if( m_configuration )
    {
        int fieldCount = m_configuration->getLogEntryFactory()->getNumberOfFields();
        for( int i = 0; i < fieldCount; i++ )
        {
            if( m_name == m_configuration->getLogEntryFactory()->getDescShort( i ) )
            {
                m_fieldId = i;
                break;
            }
        }
    }
}

void ValueGetterLogEntry::setName( const QString &name )
{
    m_name = name;

    if( m_configuration )
    {
        int fieldCount = m_configuration->getLogEntryFactory()->getNumberOfFields();
        for( int i = 0; i < fieldCount; i++ )
        {
            if( m_name == m_configuration->getLogEntryFactory()->getDescShort( i ) )
            {
                m_fieldId = i;
                break;
            }
        }
    }
}

QString ValueGetterLogEntry::getName() const
{
  return m_name;
}

TSharedConstQString ValueGetterLogEntry::getValue( TconstSharedLogEntry &entry ) const
{
    return entry->getAttributeAsString( m_fieldId );
}

bool ValueGetterLogEntry::isValid( ) const
{
    return m_fieldId >= 0;
}

int ValueGetterLogEntry::getID( ) const
{
    return m_fieldId;
}

std::ostream &ValueGetterLogEntry::out( std::ostream &o, bool extended ) const
{
    if( extended )
    {
        o << "ValueGetterLogEntry{"<< m_name.toStdString() << "}";
        return o;
    }
    else
    {
        o << m_name.toStdString();
        return o;
    }
}
