/*
 * EventFilterToBoostFunction.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: sven
 */

#include "GUITools/EventFilterToBoostFunction.h"

#include <QKeyEvent>

namespace evtFunc
{
    bool keyPressed(QObject *o, QEvent *e, int keycode, std::function<void()> f )
    {
        if (e->type() == QEvent::KeyPress )
        {
             auto *keyEvent = dynamic_cast<QKeyEvent *>(e);
             if (keyEvent != nullptr && keyEvent->key() == keycode)
             {
                 f();
                 return true;
             }
        }

        return false;
    }
}
