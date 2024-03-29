/*
 * SynchronizedHeaderView.cpp
 *
 *  Created on: Jan 5, 2012
 *      Author: sven
 */

#include "SynchronizedHeaderView.h"

#include "GUITools/SlotToBoostFunction.h"

using namespace std::placeholders;

HeaderViewGroup::~HeaderViewGroup()
{
    // There must be at least one missing, this is the one we are inside.
    // headerChildDestroyed may be called after the destructor of our shared_ptr
    Q_ASSERT( m_groupChilds.size() <= 1 );
}

void HeaderViewGroup::addToGroup( SynchronizedHeaderView *view )
{
    QObject::connect(view, SIGNAL(destroyed(QObject *)),
            this, SLOT(headerChildDestroyed(QObject *)));

    for( TGroupList::iterator it = m_groupChilds.begin()
        ; it != m_groupChilds.end()
        ; ++it )
    {
        QObject::connect( view,SIGNAL(sectionResized(int , int , int )),
                // Destruction of this will be handled by the action itself.
                new SlotToBoostFunction_int_int_int(*it,std::bind(&SynchronizedHeaderView::resizeSectionFuckingQT,*it,_1,_3)),
                SLOT(handleSignal(int,int,int)));

        QObject::connect( *it,SIGNAL(sectionResized(int , int , int )),
                // Destruction of this will be handled by the action itself.
                new SlotToBoostFunction_int_int_int(view,std::bind(&SynchronizedHeaderView::resizeSectionFuckingQT,view,_1,_3)),
                SLOT(handleSignal(int,int,int)));
    }

    QObject::connect( view,SIGNAL(sectionMoved(int , int , int )),
            // Destruction of this will be handled by the action itself.
            new SlotToBoostFunction(view,std::bind(&HeaderViewGroup::synchronizeState,this,view)),
            SLOT(handleSignal()));

    m_groupChilds.push_back( view );
}

void HeaderViewGroup::hideSection( int idx )
{
    for( TGroupList::iterator it = m_groupChilds.begin()
        ; it != m_groupChilds.end()
        ; ++it )
    {
        (*it)->hideSectionGroup( idx );
    }
}

void HeaderViewGroup::showSection( int idx )
{
    for( TGroupList::iterator it = m_groupChilds.begin()
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

    for( TGroupList::iterator it = m_groupChilds.begin()
        ; it != m_groupChilds.end()
        ; ++it )
    {
        (*it)->restoreState( view->saveState() );
    }
}

void HeaderViewGroup::headerChildDestroyed( QObject *obj )
{
    SynchronizedHeaderView *view = static_cast<SynchronizedHeaderView *>(obj);

    TGroupList::iterator it = std::find( m_groupChilds.begin(), m_groupChilds.end(), view );
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
