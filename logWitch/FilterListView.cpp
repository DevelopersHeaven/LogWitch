/*
 * FilterListView.cpp
 *
 *  Created on: Jun 4, 2011
 *      Author: sven
 */
#include "FilterListView.h"
#include "LogEntryTableWindow.h"
#include "Models/StringCacheTreeModel.h"
#include "LogData/LogEntryParserModelConfiguration.h"
#include "LogData/LogEntryFactory.h"

FilterListView::FilterListView( QObject *parent, std::shared_ptr<const LogEntryParserModelConfiguration> config, const int attr )
: QTreeView( )
, m_config( config )
, m_attr( attr )
{
    m_strModel = new StringCacheTreeModel( parent
            , &m_config->getLogEntryFactory()->getCache(attr)
            , attr
            , m_config->getHierarchySplitString(attr) );

    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(m_strModel);
    proxyModel->setSourceModel(m_strModel);
    proxyModel->sort ( 0 );
    proxyModel->setSortCaseSensitivity ( Qt::CaseInsensitive );
    proxyModel->setDynamicSortFilter ( true );

    setHeaderHidden ( true );
    setModel(proxyModel);

    // Context Menu stuff
    setContextMenuPolicy( Qt::CustomContextMenu );
    connect(this ,SIGNAL(customContextMenuRequested( const QPoint &)),
            this, SLOT(contextMenuPopup(const QPoint &)));

    m_resetSelectionsAct = new QAction(tr("reset all"), this);
    m_resetSelectionsAct->setToolTip( tr("Resets all and sets it like on startup.") );
    m_selectThisAct = new QAction(tr("select this only"), this);
    m_selectThisAct->setToolTip( tr("Selects only this item, leaves children as they are.") );
    m_deselectThisAct = new QAction(tr("deselect this only"), this);
    m_deselectThisAct->setToolTip( tr("Deselects only this item, leaves children as they are.") );
    m_selectOnlyThisWithPathAct = new QAction(tr("Select only this including path"), this);
    m_selectOnlyThisWithPathAct->setToolTip( tr("Selects only this element including the path to this element.") );
    m_selectOnlyThisWithoutPathAct = new QAction(tr("Select only this excluding path"), this);
    m_selectOnlyThisWithoutPathAct->setToolTip( tr("Selects only this element excluding the path to this element.") );
    m_checkTreeAct = new QAction(tr("Select Tree"), this);
    m_checkTreeAct->setToolTip( tr("Selects the complete subtree.") );
    m_uncheckTreeAct = new QAction(tr("Deselect Tree"), this);
    m_uncheckTreeAct->setToolTip( tr("Deselects the complete subtree.") );


    m_contextMenu = new QMenu( this );
    m_contextMenu->addAction( m_selectThisAct );
    m_contextMenu->addAction( m_deselectThisAct );

    m_contextMenu->addAction( m_selectOnlyThisWithPathAct );
    m_contextMenu->addAction( m_selectOnlyThisWithoutPathAct );
    m_contextMenu->addAction( m_checkTreeAct );
    m_contextMenu->addAction( m_uncheckTreeAct );

    m_contextMenu->addAction( m_resetSelectionsAct );

}

void FilterListView::addToTabs( QTabWidget *tabs, LogEntryTableWindow* widget  )
{
    if( m_strModel->getFilter() )
        widget->addFilter( m_strModel->getFilter() );

    tabs->addTab( this, m_config->getLogEntryFactory()->getDescLong(m_attr) );
    show();
}

void FilterListView::contextMenuPopup( const QPoint &pos)
{
    QModelIndex idx = indexAt( pos );

    QAction *triggered = m_contextMenu->exec( this->mapToGlobal(pos) );
    if( triggered == m_selectThisAct )
    {
        this->model()->setData( idx, 1, 512 );
    }
    else if( triggered == m_deselectThisAct )
    {
        this->model()->setData( idx, 0, 512 );
    }
    else if( triggered == m_selectOnlyThisWithPathAct )
    {
        this->model()->setData( idx, 2, 512 );
    }
    else if( triggered == m_selectOnlyThisWithoutPathAct )
    {
        this->model()->setData( idx, 3, 512 );
    }
    else if( triggered == m_resetSelectionsAct )
    {
        this->model()->setData( idx, 4, 512 );
    }
    else if( triggered == m_checkTreeAct )
    {
        this->model()->setData( idx, 5, 512 );
    }
    else if( triggered == m_uncheckTreeAct )
    {
        this->model()->setData( idx, 6, 512 );
    }
}

FilterListView::~FilterListView()
{
}
