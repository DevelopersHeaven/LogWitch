/*
 * LogEntryTableFilter.cpp
 *
 *  Created on: May 18, 2011
 *      Author: sven
 */

#include "LogEntryTableFilter.h"

#include <QtGui>
#include <algorithm>

#include "Models/LogEntryTableModel.h"
#include "LogData/LogEntry.h"
#include "LFAAssert.h"
#include "ActionRules/ActionDataRewriter.h"
#include "ActionRules/ActionDiscardRow.h"

LogEntryTableFilter::LogEntryTableFilter( QObject *parent)
	: QSortFilterProxyModel( parent )
	, m_model( NULL )
  , m_ruleTable( new RuleTable )
  , m_surroundingRowStart(-1)
  , m_surroundingRowEnd(-1)
  , m_resetFilterNeeded( false )
  , m_exportOfSourceModel( NULL )
{
    QObject::connect(&m_filterChain, SIGNAL(filterUpdateFinished()),
                     this, SLOT(invalidate()));
    QObject::connect( m_ruleTable.get(), SIGNAL(changed()),
            this, SLOT(updateChanges()));
}

LogEntryTableFilter::~LogEntryTableFilter()
{
}

void LogEntryTableFilter::generateExportList( std::vector<TconstSharedLogEntry>& ls
    , QModelIndex first, QModelIndex last
    , const ExportParameters& param ) const
{
  // Since we have not implemented sorting, we can use a range to fetch
  // the entries for now.
  QModelIndex srcFirst = mapToSource( first );
  QModelIndex srcLast = mapToSource( last );
  if ( param.withoutFilter )
  {
    m_exportOfSourceModel->generateExportList( ls, srcFirst, srcLast, param );
  }
  else
  {
    std::vector<TconstSharedLogEntry> tmpVec;
    m_exportOfSourceModel->generateExportList( tmpVec, srcFirst, srcLast, param );

    for ( TconstSharedLogEntry e : tmpVec )
    {
      if (filterAcceptInt(e))
        ls.push_back(e);
    }
  }
}

void LogEntryTableFilter::addFilter( std::shared_ptr<LogEntryFilter> flt )
{
	m_filterChain.addFilter( flt );
}

TSharedRuleTable LogEntryTableFilter:: getRuleTable()
{
    return m_ruleTable;
}

QVariant LogEntryTableFilter::data(const QModelIndex &index, int role) const
{
    QVariant var = QSortFilterProxyModel::data( index, role );

    const QModelIndex sourceIdx = mapToSource( index );
    if( !sourceIdx.isValid() )
    {
        qWarning() << "We had an error during mapping to source!";
        return QVariant();
    }

    TconstSharedLogEntry entry = m_model->getEntryByIndex( sourceIdx );
    std::list<  TconstSharedActionDataRewriter > actions;
    if( m_ruleTable->getMatchedActionsForType( actions, entry ) )
    {
        std::list<  TconstSharedActionDataRewriter >::iterator it;
        for( it = actions.begin(); it != actions.end(); ++it  )
            (*it)->modifyData( var, sourceIdx.column(), role);
    }

    if(  sourceIdx.row() >= m_surroundingRowStart
        && sourceIdx.row() <= m_surroundingRowEnd
        && !filterAcceptInt( entry ))
    {
        if( role == Qt::FontRole )
        {
            QFont font;
            font.setItalic(true);
            var = font;
        }
        else if( role == Qt::ForegroundRole )
        {
            var = QColor(128,128,128);
        }
    }

    return var;
}

LogEntryTableModel *LogEntryTableFilter::getSourceModel() const
{
    return m_model;
}

void LogEntryTableFilter::setSourceModel( QAbstractItemModel *model )
{
	QSortFilterProxyModel::setSourceModel( model );

  m_exportOfSourceModel = dynamic_cast<ExportableIfc *>(model);
  Q_CHECK_PTR(m_exportOfSourceModel);

	m_model = dynamic_cast<LogEntryTableModel *>( sourceModel() );
	hideSurroundingLogEntries();
	LFA_ASSERT( m_model, "Invalid model given!" );

    QObject::connect( model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &,int,int)),
            this, SLOT(slotRowsAboutToBeRemoved(const QModelIndex &,int,int)));
    QObject::connect( model, SIGNAL(rowsRemoved(const QModelIndex &,int,int)),
            this, SLOT(slotInvalidateIfNeeded()));
    QObject::connect( model, SIGNAL(modelAboutToBeReset()),
        this, SLOT(slotModelAboutToBeReset()));
    QObject::connect( model, SIGNAL(modelReset()),
        this, SLOT(slotInvalidateIfNeeded()));
}

bool LogEntryTableFilter::filterAcceptsRow( int sourceRow, const QModelIndex & ) const
{
    TconstSharedLogEntry entry = m_model->getEntryByRow( sourceRow );

    if(    sourceRow >= m_surroundingRowStart
        && sourceRow <= m_surroundingRowEnd )
    {
        return true;
    }

    return filterAcceptInt( entry );
}

bool LogEntryTableFilter::filterAcceptInt ( TconstSharedLogEntry entry ) const
{
    if( ! m_filterChain.filterEntry( entry ) )
        return false;

    TExpressionVector::const_iterator it;
    for( it = m_discardExpressions.begin(); it != m_discardExpressions.end(); ++it )
        if( (*it)->match( entry ) )
            return false;

    return true;
}

void LogEntryTableFilter::showSurroundingLogEntries( const QModelIndex &index, uint valuesToShow )
{
    qDebug() << "showSurroundingLogEntries with row: " << index.row();
    int center =  mapToSource( index ).row();
    if( center == -1 )
    {
        hideSurroundingLogEntries();
        return;
    }
    m_surroundingRowStart = std::max<int>( 0, center - valuesToShow );
    m_surroundingRowEnd = center + valuesToShow;

    qDebug() << "m_surroundingRowStart: " << m_surroundingRowStart << " m_surroundingRowEnd: " << m_surroundingRowEnd;
    invalidateFilter();
}

void LogEntryTableFilter::hideSurroundingLogEntries()
{
    m_surroundingRowStart = -1;
    m_surroundingRowEnd = -1;
    invalidateFilter();
}

void LogEntryTableFilter::slotRowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end )
{
    qDebug() << " start: " << start << " end: " << end << " m_surroundingRowStart: " << m_surroundingRowStart
            << " m_surroundingRowEnd: " << m_surroundingRowEnd;
    if( m_surroundingRowStart == -1 && m_surroundingRowEnd == -1 )
        return;

    if( start < m_surroundingRowStart )
    {
        int diff = end - start + 1;
        if( end < m_surroundingRowStart )
        {
            m_surroundingRowStart -= diff;
            m_surroundingRowEnd -= diff;
        }
        else if( end < m_surroundingRowEnd )
        {
            m_surroundingRowStart = start;
            m_surroundingRowEnd -= diff;
        }
    }
    else if( start >= m_surroundingRowStart )
    {
        if( end <= m_surroundingRowEnd )
        {
            int diff = end - start + 1;
            m_surroundingRowEnd -= diff;
        }
        else
        {
            m_surroundingRowEnd = start - 1;
        }
    }
    else
    {
        m_surroundingRowStart = -1;
        m_surroundingRowEnd = -1;
    }

    if( m_surroundingRowEnd < m_surroundingRowStart )
    {
        m_surroundingRowStart = -1;
        m_surroundingRowEnd = -1;
    }

    qDebug() << " after:  m_surroundingRowStart: " << m_surroundingRowStart
            << " m_surroundingRowEnd: " << m_surroundingRowEnd;

    m_resetFilterNeeded = true;
}

void LogEntryTableFilter::slotModelAboutToBeReset()
{
    qDebug()<<"LogEntryTableFilter::slotModelAboutToBeReset()";
    if( m_surroundingRowStart == -1 && m_surroundingRowEnd == -1 )
        return;

    m_surroundingRowStart = -1;
    m_surroundingRowEnd = -1;
    m_resetFilterNeeded = true;
}

void LogEntryTableFilter::slotInvalidateIfNeeded()
{
    qDebug() << "LogEntryTableFilter::slotInvalidateIfNeeded()";
    if( m_resetFilterNeeded )
        invalidateFilter();
    m_resetFilterNeeded = false;
}


void LogEntryTableFilter::updateChanges()
{
    TExpressionVector newExpressions = m_ruleTable->getExpressionsWithAction<TconstSharedActionDiscardRow>();
    if( newExpressions != m_discardExpressions )
    {
        // We have to invalidate the complete model here, because the discard filter has changed.
        // This takes more time, than only invalidating data.
        m_discardExpressions = newExpressions;
        hideSurroundingLogEntries();
        invalidate();
    }
    else
    {
        // This invalidates the item data only. This is much faster, because only
        // the actual viewport will be updated.
        emit dataChanged( index( 0, 0 ), index( rowCount()-1, columnCount()-1 ) );
    }
}
