/*
 * WidgetStateSaver.cpp
 *
 *  Created on: May 28, 2011
 *      Author: sven
 */

#include "WidgetStateSaver.h"
#include <QDebug>

WidgetStateSaver::WidgetStateSaver( QObject *parent )
    : QObject( parent )
{

}

void WidgetStateSaver::addElementToWatch( QObject *obj,
        std::shared_ptr<ObjectStateSavingInterface> stateSaver )
{
    DEBUG_WIDGETSTATESAFER("adding object to watch " <<obj);
    m_myWatchedObjects.insert( ObjectStateDumper::value_type( obj, stateSaver ) );
    // Save actual obj state as state default.
    auto it = m_stateHistoryMap.find(nullptr);
    if( it == m_stateHistoryMap.end() )
    {
        it = m_stateHistoryMap.insert(StateSaveMap::value_type(nullptr, ObjectStateList())).first;
    }

    it->second.push_back(stateSaver->dumpState(obj, nullptr));
}

void WidgetStateSaver::storeState( QObject *obj )
{
    DEBUG_WIDGETSTATESAFER("Storing state");
    ObjectStateDumper::iterator it;
    ObjectStateList stateList;

    StateSaveMap::iterator itSM;
    itSM = m_stateHistoryMap.insert( StateSaveMap::value_type(obj,ObjectStateList())).first;

    itSM->second.clear();

    for( it = m_myWatchedObjects.begin(); it != m_myWatchedObjects.end(); ++it )
    {
        itSM->second.push_back( it->second->dumpState( it->first, m_lastObject ) );
    }
}

void WidgetStateSaver::replayState( QObject *obj )
{
    StateSaveMap::iterator it;

    it = m_stateHistoryMap.find( obj );

    if( it != m_stateHistoryMap.end() )
    {
        qDebug() << "Replaying state";

        ObjectStateList::iterator itStates;
        for( itStates = it->second.begin(); itStates != it->second.end(); ++itStates )
        {
            (*itStates)->replayState( m_lastObject );
        }
    }
}

void WidgetStateSaver::deregisterFocusObject( QObject *end, bool applyDefauls )
{
    DEBUG_WIDGETSTATESAFER("DeregisterFocusObject" << end);
    if( applyDefauls )
        replayState(nullptr);

    m_stateHistoryMap.erase(end );
}

void WidgetStateSaver::switchState( QObject *newObject)
{
    DEBUG_WIDGETSTATESAFER("Request for wwitch state from " << m_lastObject << " to new " << newObject);
    if( m_lastObject == newObject )
        return;
    DEBUG_WIDGETSTATESAFER("Doing Switch state from " << m_lastObject << " to new " << newObject);

    if( m_lastObject )
        storeState( m_lastObject );
    m_lastObject = newObject;
    replayState( newObject );
}
