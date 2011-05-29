/*
 * SignalMultiplexerStateApplier.cpp
 *
 *  Created on: May 29, 2011
 *      Author: sven
 */

#include "SignalMultiplexerStateApplier.h"
#include "WidgetStateSaver.h"
#include "GUITools/SignalMultiplexer.h"

boost::shared_ptr<SignalMultiplexerStateApplier> SignalMultiplexerStateApplier::generate( SignalMultiplexer *mul)
{
    shared_ptr<SignalMultiplexerStateApplier> obj( new SignalMultiplexerStateApplier( mul ) );
    obj->m_ptrToMyself = obj;

    return obj;
}

SignalMultiplexerStateApplier::SignalMultiplexerStateApplier( SignalMultiplexer *mul)
    : m_multiplexer( mul )
{

}

boost::shared_ptr<ObjectState> SignalMultiplexerStateApplier::dumpState( QObject *, QObject * ) const
{
    return boost::shared_ptr<ObjectState>( new ObjectState( m_ptrToMyself.lock(), NULL ) );
}

void SignalMultiplexerStateApplier::replayState( QObject *, QObject *parent, const ObjectState * ) const
{
    m_multiplexer->setObject( parent );
}

