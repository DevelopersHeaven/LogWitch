/*
 * WidgetStateSaver.h
 *
 *  Created on: May 28, 2011
 *      Author: sven
 */

#ifndef WIDGETSTATESAVER_H_
#define WIDGETSTATESAVER_H_

#include <map>
#include <list>

#include <memory>
#include <qobject.h>

#define DEBUG_WIDGETSTATESAFER( x ) ;

class ObjectState;

class ObjectStateSavingInterface
{
public:
	virtual ~ObjectStateSavingInterface() = default;

	/**
	 * An implementation should dump the state to the WidgetState here.
	 */
	virtual std::shared_ptr<ObjectState> dumpState( QObject *objectToDump, QObject *parentalObject ) const = 0;

	/**
	 * Replays the previous saved state to the object.
	 */
	virtual void replayState( QObject *objectToReplay, QObject *parentalObject, const ObjectState *state ) const = 0;
};

/**
 * This is the base class for storing the state for the widgets.
 */
class ObjectState
{
public:
	ObjectState() : m_isValid( false ) { }
	ObjectState( std::shared_ptr<const ObjectStateSavingInterface> replayer, QObject *obj ) : m_isValid( true ), m_object( obj ), m_replayer( replayer ) { }

	virtual ~ObjectState() = default;

	bool isValid() const{ return m_isValid; }

	void replayState( QObject *parent ) const { if( isValid() ) m_replayer->replayState( m_object, parent, this ); }

protected:
	bool m_isValid;

	QObject *m_object;

	std::shared_ptr<const ObjectStateSavingInterface> m_replayer;
};



/**
 * This class watches the focus of the mdi sub windows and
 * saves their states of some widgets if the focus is changed.
 *
 * For default is saves only the state of the inside widget.
 */

class WidgetStateSaver: public QObject {
public:
	WidgetStateSaver( QObject *parent );

	/**
	 * This adds an element to exchange the inner widgets / states.
	 * @param obj Object to dump the state for
	 * @param stateSaver The Class which knows how to dump and restore the state.
	 */
	void addElementToWatch( QObject *obj, std::shared_ptr<ObjectStateSavingInterface> stateSaver );

	/**
	 * Call this if the window gets out of focus to save the state.
	 */
	void storeState( QObject *obj );

	/**
	 * If you enter the focus of an object, call this and the
	 * previous saved state will be restored.
	 */
	void replayState( QObject *obj );

	/**
	 * This deregisters a watched object and removes all watched objects if they were present.
	 * This method must be called if a window is deleted for example.
	 */
	void deregisterFocusObject( QObject *end, bool applyDefaults = true );

	void switchState( QObject *newObject);

private:
	typedef std::map<QObject *, std::shared_ptr<ObjectStateSavingInterface> > ObjectStateDumper;

	/**
	 * This list contains the watched objects with their corresponding stateDumpers.
	 * This list will be worked out if the storeState is called.
	 */
	ObjectStateDumper m_myWatchedObjects;

	typedef std::list< std::shared_ptr<ObjectState> > ObjectStateList;

	typedef std::map<QObject *, ObjectStateList> StateSaveMap;

	/*
	 * This is the map to save states to. The first index is the object which is watched
	 * (a subwindow for example), the second are the different states with their saved state.
	 */
	StateSaveMap m_stateHistoryMap;

	QObject *m_lastObject = nullptr;
};

#endif /* WIDGETSTATESAVER_H_ */
