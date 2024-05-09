/*
 * ActionColorRow.cpp
 *
 *  Created on: Jun 5, 2011
 *      Author: sven
 */

#include "ActionDataRewriter.h"

#include <utility>

#include "LogData/LogEntryFactory.h"

ActionDataRewriter::ActionDataRewriter(TSharedConstLogEntryParserModelConfiguration configuration)
: m_cfg(std::move( configuration ))
{
}

bool ActionDataRewriter::modifyData( QVariant &var,  int column, int role) const
{
    TChangeSet::const_iterator it = m_changes.find( rc_key(role,column) );
    if( it == m_changes.end() )
        it = m_changes.find( rc_key(role) );

    if( it != m_changes.end() )
    {
        var = it->second;
        return true;
    }

    return false;
}

QVariant ActionDataRewriter::toDisplay( int role ) const
{
    QVariant variant;

    if( role == Qt::DisplayRole )
    {
        variant = QString( QObject::tr("Text changing") );
    }

    TChangeSet::const_iterator it;

    for( it = m_changes.begin(); it != m_changes.end(); ++it )
    {
        if( it->first.role == role )
        {
            variant = it->second;
        }
    }

    return variant;
}

void ActionDataRewriter::addChangeSet( const QVariant &var, int role )
{
    m_changes.insert( TChangeSet::value_type(rc_key(role),var) );
}

void ActionDataRewriter::addChangeSet( const QVariant &var, int role, const QString &column )
{
    if( m_cfg )
    {
        int fieldCount = m_cfg->getLogEntryFactory()->getNumberOfFields();
        for( int i = 0; i < fieldCount; i++ )
        {
            if( column == m_cfg->getLogEntryFactory()->getDescShort( i ) )
            {
                m_changes.insert( TChangeSet::value_type(rc_key(role, i),var) );
                break;
            }
        }
    }
    else
    {
        // this is a fallback  if the configuration is missing. This is the case if we
        // use the action only for displaying the action.
        addChangeSet( var, role );
    }
}

