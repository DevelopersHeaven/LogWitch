/*
 * SynchronizedHeaderView.cpp
 *
 *  Created on: Jan 5, 2012
 *      Author: sven
 */

#include "SynchronizedHeaderView.h"

#include <QDebug>

#include "GUITools/SlotToBoostFunction.h"

using namespace std::placeholders;

void HeaderViewGroup::addToGroup( SynchronizedHeaderView *view )
{
    QObject::connect(view, &QObject::destroyed,
                     this, &HeaderViewGroup::headerChildDestroyed);

    for( auto it = m_groupChilds.begin()
        ; it != m_groupChilds.end()
        ; ++it )
    {
        QObject::connect(view, &QHeaderView::sectionResized,
                // Destruction of this will be handled by the action itself.
                new SlotToBoostFunction_int_int_int(*it,std::bind(&SynchronizedHeaderView::resizeSectionFuckingQT,*it,_1,_3)),
                &SlotToBoostFunction_int_int_int::handleSignal);

        QObject::connect(*it, &QHeaderView::sectionResized,
                // Destruction of this will be handled by the action itself.
                new SlotToBoostFunction_int_int_int(view,std::bind(&SynchronizedHeaderView::resizeSectionFuckingQT,view,_1,_3)),
                &SlotToBoostFunction_int_int_int::handleSignal);
    }

    QObject::connect(view, &QHeaderView::sectionMoved,
            // Destruction of this will be handled by the action itself.
            new SlotToBoostFunction(view,std::bind(&HeaderViewGroup::synchronizeState,this,view)),
            &SlotToBoostFunction::handleSignal);

    m_groupChilds.push_back( view );
}

void HeaderViewGroup::hideSection( int idx )
{
    for( auto it = m_groupChilds.begin()
        ; it != m_groupChilds.end()
        ; ++it )
    {
        (*it)->hideSectionGroup( idx );
    }
}

void HeaderViewGroup::showSection( int idx )
{
    for( auto it = m_groupChilds.begin()
        ; it != m_groupChilds.end()
        ; ++it )
    {
        (*it)->showSectionGroup( idx );
    }
}

void HeaderViewGroup::synchronizeState( SynchronizedHeaderView *view )
{
    // Synchronizing with the signals itself will not work, because the
    // Header view will emit a signal by itself. This signal cannot
    // be blocked, because otherwise the other views will get into
    // trouble. So restoring the state is the working way.

    for( auto it = m_groupChilds.begin()
        ; it != m_groupChilds.end()
        ; ++it )
    {
        (*it)->restoreState( view->saveState() );
    }
}

void HeaderViewGroup::headerChildDestroyed( QObject *obj )
{
    auto it = std::find(m_groupChilds.begin(), m_groupChilds.end(), obj);
    if( it != m_groupChilds.end() )
        m_groupChilds.erase( it );
    else
    {
        qDebug()<<"HeaderViewGroup::headerChildDestroyed: Error: Unable to find object!!!";
    }
}

SynchronizedHeaderView *HeaderViewGroup::getMaster()
{
    Q_ASSERT( !m_groupChilds.empty() );
    return m_groupChilds.front();
}

SynchronizedHeaderView::SynchronizedHeaderView( SynchronizedHeaderView *syncTo, Qt::Orientation orientation, QWidget * parent )
: QHeaderView( orientation, parent )
{
    if( syncTo )
    {
        m_group = syncTo->m_group;
        m_group->addToGroup( this );
    }
    else
    {
        m_group.reset( new HeaderViewGroup );
        m_group->addToGroup( this );
    }
}

void SynchronizedHeaderView::resizeSectionFuckingQT( int logicalIndex, int size )
{
    if( size == 0 )
    {
        hideSection( logicalIndex );
    }
    else
        resizeSection( logicalIndex, size );
}

void SynchronizedHeaderView::synchronize( SynchronizedHeaderView *syncFrom )
{
    if( !syncFrom )
        syncFrom = m_group->getMaster();

    if( syncFrom == this )
        return;

    this->restoreState( syncFrom->saveState() );
    this->restoreGeometry( syncFrom->saveGeometry() );
}

void SynchronizedHeaderView::hideSection( int idx )
{
    QHeaderView::hideSection( idx );
    m_group->hideSection( idx );
}

void SynchronizedHeaderView::showSection( int idx )
{
    QHeaderView::showSection( idx );
    m_group->showSection( idx );
}


void SynchronizedHeaderView::hideSectionGroup( int idx )
{
    QHeaderView::hideSection( idx );
}

void SynchronizedHeaderView::showSectionGroup( int idx )
{
    QHeaderView::showSection( idx );
}
