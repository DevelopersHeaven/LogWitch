/*
 * EventFilterToBoostFunction.h
 *
 *  Created on: Jan 4, 2012
 *      Author: sven
 */

#ifndef EVENTFILTERTOBOOSTFUNCTION_H_
#define EVENTFILTERTOBOOSTFUNCTION_H_

#include <QtCore>

/**
 * This class makes event filtering more easy, it maps the eventFilter to a more
 * easy to implement function, which can also be used with std::bind.
 */
class EventFilterToBoostFunction
: public QObject
{
    Q_OBJECT
public:
    EventFilterToBoostFunction( QObject * parent, std::function< bool (QObject *, QEvent *)> const & func):
        QObject( parent ), m_func( func )
    {  }

protected:
    bool eventFilter(QObject *obj, QEvent *event)
    {
        return m_func( obj, event );
    }

private:
    std::function< bool (QObject *, QEvent *)> m_func;
};

namespace evtFunc
{
    /**
     * This function is a filter object, which can be used with the event filter.
     * This object executes f if the event is a keyPressed event with the given
     * keycode.
     */
    bool keyPressed(QObject *o, QEvent *e, int keycode, std::function<void()> f );
}

#endif /* EVENTFILTERTOBOOSTFUNCTION_H_ */
