/*
 * SignalMultiplexerStateApplier.cpp
 *
 *  Created on: May 29, 2011
 *      Author: sven
 */

#include "SignalMultiplexerStateApplier.h"
#include "WidgetStateSaver.h"
#include "GUITools/SignalMultiplexer.h"

std::shared_ptr<SignalMultiplexerStateApplier> SignalMultiplexerStateApplier::generate( SignalMultiplexer *mul)
{
    std::shared_ptr<SignalMultiplexerStateApplier> obj( new SignalMultiplexerStateApplier( mul ) );
    obj->m_ptrToMyself = obj;

    return obj;
}

SignalMultiplexerStateApplier::SignalMultiplexerStateApplier( SignalMultiplexer *mul)
    : m_multiplexer( mul )
{

}

std::shared_ptr<ObjectState> SignalMultiplexerStateApplier::dumpState( QObject *, QObject * ) const
{
    return std::shared_ptr<ObjectState>(new ObjectState(m_ptrToMyself.lock(), nullptr));
}

void SignalMultiplexerStateApplier::replayState( QObject *, QObject *parent, const ObjectState * ) const
{
    m_multiplexer->setObject( parent );
}

