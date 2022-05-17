/*
 * SignalMultiplexerStateApplier.h
 *
 *  Created on: May 29, 2011
 *      Author: sven
 */

#ifndef SIGNALMULTIPLEXERSTATEAPPLIER_H_
#define SIGNALMULTIPLEXERSTATEAPPLIER_H_

#include "WidgetStateSaver.h"

class QWidget;
class SignalMultiplexer;

class SignalMultiplexerStateApplier
    : public ObjectStateSavingInterface
{
private:
        SignalMultiplexerStateApplier( SignalMultiplexer *);

public:
    static std::shared_ptr<SignalMultiplexerStateApplier> generate( SignalMultiplexer *);

public:
    std::shared_ptr<ObjectState> dumpState( QObject *obj, QObject * ) const;
    void replayState( QObject *obj, QObject *, const ObjectState *state ) const;

private:

    std::weak_ptr<SignalMultiplexerStateApplier> m_ptrToMyself;

    SignalMultiplexer *m_multiplexer;
};

#endif /* SIGNALMULTIPLEXERSTATEAPPLIER_H_ */
