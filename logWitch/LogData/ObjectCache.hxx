/*
 * StringCache.h
 *
 *  Created on: May 13, 2011
 *      Author: sven
 */

#ifndef OBJECTCACHE_H_
#define OBJECTCACHE_H_
#include <set>
#include <QtCore/QObject>
#include "Types.h"

/**
 * This is a cache signaller class compatible with QT using const QString
 * as storage type.
 * This is a workaround, because QT does not support QBobject within templates.
 */
class ObjectCacheQStringSignaller
    : public QObject
{
    Q_OBJECT
public:
    typedef const QString StType;
    typedef std::shared_ptr<StType> shStType;

signals:
    // This is not working in QT, typedefs must be the same!
    // void newElement( shStType );
    void newElement( TSharedConstQString );
};

template<class SIGNALLER>
class GetObjectIF
	: public SIGNALLER
{
public:
    typedef typename SIGNALLER::shStType StType;

    StType virtual getObject( StType in ) = 0;

    bool virtual isCaching() const = 0;
};

/**
 * This class caches strings for usage. The class is a container
 * which gives a new string or a cached ptr to the already saved string.
 * This class ensures there is always one string instance for a specific
 * string present.
 */
template<class SIGNALLER>
class ObjectCache
	: public GetObjectIF<SIGNALLER>
{
	typedef typename SIGNALLER::StType OBJ;
public:
	typedef typename SIGNALLER::shStType StType;
	ObjectCache();

	/**
	 * Returns the cached object if in is already within the cache, otherwise
	 * in will be added to the cache and returned.
	 * @param in Object passing in
	 */
	StType getObject( StType in );

	bool isCaching() const{ return true;}

private:
	struct lessStringSharedPtr
	{
	  bool operator() (const StType &s1, const StType &s2) const
	  {return (*s1) < (*s2);}
	};

	typedef std::set< StType, typename ObjectCache<SIGNALLER>::lessStringSharedPtr > TMyCache;

	TMyCache cache;
};

/**
 * This class does not do any caching, it just passes the got object through.
 */
template<class SIGNALLER>
class ObjectPasser
    : public GetObjectIF<SIGNALLER>
{
    typedef typename SIGNALLER::StType OBJ;
public:
    typedef typename SIGNALLER::shStType StType;

    /**
     * Returns the cached object if in is already within the cache, otherwise
     * in will be added to the cache and returned.
     * @param in Object passing in
     */
    StType getObject( StType in );

    bool isCaching() const {return false;};
};


template<class SIGNALLER>
ObjectCache<SIGNALLER>::ObjectCache()
{ }

template<class SIGNALLER>
typename ObjectCache<SIGNALLER>::StType ObjectCache<SIGNALLER>::getObject( typename ObjectCache<SIGNALLER>::StType str )
{
    std::pair<typename TMyCache::iterator,bool> rv = cache.insert( str );
    if( rv.second ) // str new to container, so invoke update request
        emit this->newElement( *(rv.first) );

    return *(rv.first);
}

template<class SIGNALLER>
typename ObjectPasser<SIGNALLER>::StType ObjectPasser<SIGNALLER>::getObject( typename ObjectPasser<SIGNALLER>::StType str )
{
    return str;
}

#endif /* STRINGCACHE_H_ */
