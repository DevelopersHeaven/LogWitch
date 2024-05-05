/*
 * SignalMultiplexer.cpp
 *
 *  Created on: May 29, 2011
 *      Author: sven
 *
 */
#include "SignalMultiplexer.h"

SignalMultiplexer::SignalMultiplexer(QObject *parent )
    : QObject( parent )
    , m_object(nullptr)
{
}

void SignalMultiplexer::connect(QObject *sender, const char *signal, QObject *receiver, const char *slot)
{
    connectionState state( sender, signal, receiver, slot );
    StateConnectionsForObjects::iterator it = m_connectionStates.find( m_object );
    if( it == m_connectionStates.end() )
    {
        it = m_connectionStates.insert(
                StateConnectionsForObjects::value_type( (QObject *)m_object, TConnectionList() ) ).first;

        QObject::connect(m_object, SIGNAL(destroyed(QObject *)),
                             this, SLOT(deleteObject(QObject *)));
    }
    it->second.push_back( state );
    connect( state );
}

bool SignalMultiplexer::disconnect(QObject *sender, const char *signal, QObject *receiver, const char *slot)
{
    connectionState state( sender, signal, receiver, slot );
    StateConnectionsForObjects::iterator it = m_connectionStates.find( m_object );
    if( it == m_connectionStates.end() )
        return false;

    TConnectionList &stateList = it->second;
    TConnectionList::iterator itConnection;

    for( itConnection = stateList.begin(); itConnection != stateList.end() ; ++itConnection )
    {
        if( (*itConnection) == state )
        {
            disconnect( *itConnection );
            stateList.erase( itConnection );
            return true;
        }
    }
    return false;
}

void SignalMultiplexer::connect(const connectionState &state)
{
    if (!m_object || !state.dest || !state.src )
        return;

    QObject::connect( (QObject *)state.src, state.signalName, (QObject *)state.dest, state.slotName );
}

void SignalMultiplexer::disconnect(const connectionState &state)
{
    if (!m_object || !state.dest || !state.src )
        return;

    QObject::disconnect( (QObject *)state.src, state.signalName, (QObject *)state.dest, state.slotName );
}

void SignalMultiplexer::deleteObject( QObject *obj )
{
    StateConnectionsForObjects::iterator it = m_connectionStates.find( (QObject *)obj );
    if( it != m_connectionStates.end() )
    {
        if( obj == m_object )
        {
            setObject(nullptr);
        }

        m_connectionStates.erase( it );
    }
}

void SignalMultiplexer::setObject(
        QObject *obj)
{
    if (obj == m_object)
        return;

    StateConnectionsForObjects::iterator it = m_connectionStates.find( (QObject *)m_object );
    if( it != m_connectionStates.end() )
    {
        for ( connectionState & s : it->second )
        {
            disconnect( s );
        }
    }

    m_object = obj;

    it = m_connectionStates.find( (QObject *)m_object );
    if( it != m_connectionStates.end() )
    {
        for ( connectionState & s : it->second )
        {
            connect( s );
        }
    }

}

