/*
 * EventFilterToBoostFunction.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: sven
 */

#include "GUITools/EventFilterToBoostFunction.h"

#include <QtGui>

namespace evtFunc
{
    bool keyPressed(QObject *o, QEvent *e, int keycode, std::function<void()> f )
    {
        if (e->type() == QEvent::KeyPress )
        {
             QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
             if( keyEvent->key() == keycode )
             {
                 f();
                 return true;
             }
        }

        return false;
    }
}
