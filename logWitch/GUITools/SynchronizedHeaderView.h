/*
 * SynchronizedHeaderView.h
 *
 *  Created on: Jan 5, 2012
 *      Author: sven
 */

#ifndef SYNCHRONIZEDHEADERVIEW_H_
#define SYNCHRONIZEDHEADERVIEW_H_

#include <list>
#include <QtGui>
#include <QHeaderView>

class HeaderViewGroup;

/**
 * This is a header view which has synchronized hide/delete, section move and resize.
 */
class SynchronizedHeaderView
: public QHeaderView
{
public:
    /**
     * If you have nothing to synchronize (first element in group), just set syncTo to 0. Then
     * automatically a new group will be created for you.
     */
    SynchronizedHeaderView( SynchronizedHeaderView *syncTo, Qt::Orientation orientation, QWidget * parent = 0 );

    /**
     * Initial synchronization must be done by hand.
     */
    void synchronize( SynchronizedHeaderView *syncFrom = 0 );

    void hideSection( int idx );

    void showSection( int idx );

    /**
     * Becuase QT does not make resizeSection virtual, we have tu use this method!
     */
    void resizeSectionFuckingQT( int logivalIndex, int size );


    // Should be only called by the group.
public:
    void showSectionGroup( int idx );

    void hideSectionGroup( int idx );



private:
    std::shared_ptr<HeaderViewGroup> m_group;
};

/**
 * This is the class for binding the header together. This is just an implementation
 * detail.
 */
class HeaderViewGroup
: public QObject
{
    Q_OBJECT
public:
    HeaderViewGroup();

    virtual ~HeaderViewGroup();

    void addToGroup( SynchronizedHeaderView * );

    SynchronizedHeaderView *getMaster();

    void synchronizeState( SynchronizedHeaderView *view );

    void hideSection( int idx );

    void showSection( int idx );

public slots:
    void headerChildDestroyed( QObject * );

private:
    void updateFrom( SynchronizedHeaderView * );

public:
    typedef std::list< SynchronizedHeaderView * > TGroupList;
    TGroupList m_groupChilds;

    QMutex m_positionMutex;
    bool m_positionSyncNeeded;
};

#endif /* SYNCHRONIZEDHEADERVIEW_H_ */
