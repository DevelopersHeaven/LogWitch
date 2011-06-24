/*
 * DockWidgetStateSaver.h
 *
 *  Created on: May 28, 2011
 *      Author: sven
 */

#ifndef DOCKWIDGETSTATESAVER_H_
#define DOCKWIDGETSTATESAVER_H_

#include <boost/enable_shared_from_this.hpp>
#include <QtGui>
#include <QtCore>

#include "WidgetStateSaver.h"

/**
 * This is the type definition template to use the GetSetStateSaver for the
 * QDockWidget class. It saves the widget inside it.
 */
struct DockWidgetStateSaverTypes
{
public:
    typedef QDockWidget ObjectToSet;
    typedef QWidget* value;

    static value get( ObjectToSet *obj ) { return obj->widget(); }
    static void set( ObjectToSet *obj, value s ) { obj->setWidget( s ); }
};

/**
 * This class is a get set state saver. This state saver works with object
 * getter/setter of the type T::ObjectToSet. This is the object from which
 * the state should be saved. The object type to save is T::value.
 * The GetSetStateSaver uses the static function value *T::get( T::ObjectToSet *)
 * to retrieve the object and the static function void set( ObjectToSet *obj, value *s )
 * to restore the saved state.
 */
template<class T>
class GetSetStateSaver
	: public ObjectStateSavingInterface
	, public boost::enable_shared_from_this<GetSetStateSaver<T> >
{
private:
	GetSetStateSaver();

public:
	static boost::shared_ptr<GetSetStateSaver<T> > generate();

public:
    boost::shared_ptr<ObjectState> dumpState( QObject *obj, QObject * ) const;
    void replayState( QObject *obj, QObject *, const ObjectState *state ) const;

private:
    class state:
        public ObjectState
    {
        friend class GetSetStateSaver<T>;
    public:

        state( boost::shared_ptr<const ObjectStateSavingInterface> ptr, QObject *dock, typename T::value widget )
            : ObjectState( ptr, dock ), m_widget( widget ) { }
    private:
        typename T::value m_widget;
    };
};

template<class T>
boost::shared_ptr<GetSetStateSaver<T> > GetSetStateSaver<T>::generate()
{
    shared_ptr<GetSetStateSaver<T> > obj( new GetSetStateSaver<T>() );
    return obj;
}

template<class T>
GetSetStateSaver<T>::GetSetStateSaver()
{
}

template<class T>
boost::shared_ptr<ObjectState> GetSetStateSaver<T>::dumpState( QObject *obj, QObject * ) const
{
    DEBUG_WIDGETSTATESAFER("dumping state" << obj);

    typename T::ObjectToSet *wi = dynamic_cast<typename T::ObjectToSet *>(obj);
    if( wi )
    {
        DEBUG_WIDGETSTATESAFER( "Saving old widget: " << T::get( wi ));
        return boost::shared_ptr<ObjectState>(new state( this->shared_from_this(), obj, T::get( wi ) ));
    }
    else
    {
        DEBUG_WIDGETSTATESAFER("Ignoring: Cast failed");
        return boost::shared_ptr<ObjectState>(new ObjectState());
    }
}

template<class T>
void GetSetStateSaver<T>::replayState( QObject *obj, QObject *, const ObjectState *stateP ) const
{
    DEBUG_WIDGETSTATESAFER("Replaying state" << obj);

    typename T::ObjectToSet *wi = dynamic_cast<typename T::ObjectToSet *>(obj);
    const state *st = dynamic_cast<const state *>(stateP);

    if( wi && st )
    {
        DEBUG_WIDGETSTATESAFER("Replaying old widget: " << st->m_widget);
        T::set( wi, st->m_widget );
    }
    else
        DEBUG_WIDGETSTATESAFER("Ignoring: Cast failed");
}

typedef GetSetStateSaver<DockWidgetStateSaverTypes> DockWidgetStateSaver;

#endif /* DOCKWIDGETSTATESAVER_H_ */
