/*
 * LogEntryTableModel.cpp
 *
 *  Created on: May 14, 2011
 *      Author: sven
 */

#include "LogEntryTableModel.h"

#include <QtCore/QVariant>
#include <QDateTime>
#include <QDebug>

#include "LogData/LogEntry.h"
#include "LogData/LogEntryFactory.h"
#include "LogData/LogEntryParser.h"
#include "LogData/LogEntryParserModelConfiguration.h"

LogEntryTableModel::LogEntryTableModel( std::shared_ptr<LogEntryParser> parser )
	: m_table( )
	, m_modelConfiguration( parser->getParserModelConfiguration() )
	, m_entryLoader( parser )
	, m_ModelName("Untitled")
    , m_captureActive( true )
    , m_maxNumberOfEntries( 0 )
    , m_blockInsertingMessages( false )
{
    QObject::connect(dynamic_cast<QObject*>(parser.get()), SIGNAL(newEntry(TconstSharedNewLogEntryMessage)),
                     this, SLOT(insertEntry(TconstSharedNewLogEntryMessage))
                     , Qt::QueuedConnection );
    QObject::connect(dynamic_cast<QObject*>(parser.get()), SIGNAL(signalError(QString)),
                     this, SLOT(signalErrorFromParser(QString)) );

    m_ModelName = parser->getName();
}

void LogEntryTableModel::startModel()
{
	m_entryLoader->startEmiting();
}

void LogEntryTableModel::generateExportList( std::vector<TconstSharedLogEntry>& entries
    , QModelIndex first, QModelIndex last
    , const ExportParameters& ) const
{
  std::lock_guard<std::recursive_mutex> lo(m_mutex);
  int rowFirst = std::max( first.row(), 0  );
  int rowLast = std::min( last.row(), int( m_table.size() ) );

  for( int row = rowFirst; row <= rowLast; ++row )
    entries.push_back( m_table[row] );
}

void LogEntryTableModel::signalErrorFromParser( QString error )
{
    qDebug() << "error from parser received: " << error;
    emit signalError( error );
}

TSharedConstLogEntryParserModelConfiguration LogEntryTableModel::getParserModelConfiguration() const
{
	return m_modelConfiguration;
}

int LogEntryTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    std::lock_guard<std::recursive_mutex> lo(m_mutex);

    return m_table.size();
}

int LogEntryTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    std::lock_guard<std::recursive_mutex> lo(m_mutex);

    int value = m_modelConfiguration->getLogEntryFactory()->getNumberOfFields( );
    return value;
}

TconstSharedLogEntry LogEntryTableModel::getEntryByRow( const int row ) const
{
    std::lock_guard<std::recursive_mutex> lo(m_mutex);
    return m_table[row];
}

TconstSharedLogEntry LogEntryTableModel::getEntryByIndex( const QModelIndex &index ) const
{
    std::lock_guard<std::recursive_mutex> lo(m_mutex);

    if (index.row() < 0
      || index.row() >= int(m_table.size() ) )
    {
      qDebug() << "Returning empty item from model: index.column():" << index.column()
              << " index.row():" << index.row()
              << " table size:" << int(m_table.size() )
              << " model columns:" << (m_modelConfiguration->getLogEntryFactory()->getNumberOfFields( ) );
      return TconstSharedLogEntry();
    }

    return m_table[index.row()];
}

QVariant LogEntryTableModel::data(const QModelIndex &index, int role) const
{
    std::lock_guard<std::recursive_mutex> lo(m_mutex);

    if (!index.isValid())
        return QVariant();

    if (index.column() >= (m_modelConfiguration->getLogEntryFactory()->getNumberOfFields( ) )
    		|| index.column() < 0
    		|| index.row() < 0
    		|| index.row() >= int(m_table.size() ) )
        return QVariant();

    if (role == Qt::DisplayRole)
    {
    	TconstSharedLogEntry entry = m_table[index.row()];
    	const QVariant &var = entry->getAttribute( index.column() );
    	if( var.canConvert<TSharedConstQString>() )
    	{
    	    // Return native QString instead our special pointer.
    	    QString rv = *var.value<TSharedConstQString>();
    	    return QVariant(rv );
    	}

    	return var;
    }
    else if (role == RawDataRole)
    {
    	TconstSharedLogEntry entry = m_table[index.row()];
      return QVariant::fromValue( entry );
    }

    return QVariant();
}

QVariant LogEntryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section >= (m_modelConfiguration->getLogEntryFactory()->getNumberOfFields( ) ) || section < 0)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
		if (orientation == Qt::Horizontal)
		{
			return m_modelConfiguration->getLogEntryFactory()->getDescLong( section );
		}
    }
    else if( role == 512 )
    {
    	int rVal = 150;

  		rVal = m_modelConfiguration->getFieldWidthHint( section );

    	return rVal;
    }
    else if( role == 513 )
    {
        bool rVal = m_modelConfiguration->getFieldShowHint( section );
        return rVal;
    }
    else if( role == 514 )
    {
        return m_modelConfiguration->getFieldOrderHint( section );
    }

    return QVariant();
}

bool LogEntryTableModel::setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role )
{
    if (section >= (m_modelConfiguration->getLogEntryFactory()->getNumberOfFields( ) )
        || section < 0
        || orientation != Qt::Horizontal )
        return false;

    if( role == 512 )
    {
        m_modelConfiguration->setFieldWidthHint( section, value.value<int>(), false );

        // We do not need to emit this, because we change only the default values for the model.
        // emit headerDataChanged( orientation, section, section );
        return true;
    }
    else if( role == 513 )
    {
        m_modelConfiguration->setFieldShowHint( section, value.toBool(), false );
        return true;
    }
    else if( role == 514 )
    {
        m_modelConfiguration->setFieldOrderHint( section, value.toInt(), false );
        return true;
    }

    return false;
}

void LogEntryTableModel::clearTable()
{
    std::lock_guard<std::recursive_mutex> lo(m_mutex);
    beginResetModel();
    m_table.clear();
    endResetModel();
}

void LogEntryTableModel::insertEntry( TconstSharedNewLogEntryMessage mess )
{
    if( !m_captureActive || mess->entries.empty() )
        return;

    std::lock_guard<std::recursive_mutex> lo(m_mutex);

    if( m_blockInsertingMessages )
    {
        m_blockedInsertedMessages.push_back( mess );
        return;
    }
    size_t entryCount = mess->entries.size();

    if( m_maxNumberOfEntries > 0 )
    {
        // Remove elements to match maxEntriesFirst.
        const int toRemove = m_table.size() + entryCount - m_maxNumberOfEntries;
        if( toRemove > 0 )
        {
            if( size_t( toRemove ) >= m_table.size() )
            {
                // We will clear the table and add as much elements as we can add to our model
                beginResetModel();
                const int frontEntriesDropped = entryCount - m_maxNumberOfEntries;
                Q_ASSERT( frontEntriesDropped >= 0 );
                m_table.clear();

                std::list<TSharedLogEntry>::const_iterator it;
                int i = 0;
                for( it = mess->entries.begin(); i < frontEntriesDropped; ++i, ++it )
                    ;

                m_table.insert( m_table.end()
                        , it
                        , mess->entries.end() );
                endResetModel();

                return;
            }
            else
            {
                removeRows_unlocked( 0, toRemove );
            }
        }
    }

    const int newPos = m_table.size();
    beginInsertRows( QModelIndex(), newPos, newPos + entryCount - 1 );
    m_table.insert( m_table.end(), mess->entries.begin(), mess->entries.end() );
    endInsertRows();
}

void LogEntryTableModel::beginBlockItems()
{
    m_blockInsertingMessages = true;
}

void LogEntryTableModel::endBlockItems()
{
    m_blockInsertingMessages = false;

    while( !m_blockedInsertedMessages.empty() )
    {
        insertEntry( m_blockedInsertedMessages.front() );
        m_blockedInsertedMessages.pop_front();
    }
}

bool LogEntryTableModel::removeRows ( int row, int count, const QModelIndex & parent )
{
    std::lock_guard<std::recursive_mutex> lo(m_mutex);
    return removeRows_unlocked ( row, count, parent );
}

bool LogEntryTableModel::removeRows_unlocked ( int row, int count, const QModelIndex & parent )
{
    if( row + count > int(m_table.size() ) || count == 0 )
        return false;

    beginRemoveRows( parent, row, row + count - 1 );
    m_table.erase( m_table.begin(), m_table.begin() + count );
    endRemoveRows();
    return true;
}

void LogEntryTableModel::setMaxEntries( int max )
{
    m_maxNumberOfEntries = max;
}

void LogEntryTableModel::capture( bool active )
{
    m_captureActive = active;
}

std::any LogEntryTableModel::getLock()
{
    return std::make_shared<std::lock_guard<std::recursive_mutex>>(m_mutex);
}
