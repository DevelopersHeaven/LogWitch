/*
 * LogEntryRemoveFilter.h
 *
 *  Created on: May 18, 2011
 *      Author: sven
 */

#ifndef LOGENTRYREMOVEFILTER_H_
#define LOGENTRYREMOVEFILTER_H_
#include "LogEntryFilter.h"
#include <set>

class QString;

struct ltShPtr
{
  template<class T> bool operator()(const std::shared_ptr<T> &s1, const std::shared_ptr<T> &s2) const
  {
    return s1.get() < s2.get();
  }
};

class LogEntryRemoveFilter
	:public LogEntryFilter
{
public:
	LogEntryRemoveFilter( int attrId );
	~LogEntryRemoveFilter();

	void addEntry( std::shared_ptr<const QString> str );

	void removeEntry( std::shared_ptr<const QString> str );

	void clear();

	virtual bool filterEntry( TconstSharedLogEntry entry ) const;

private:
	std::set<std::shared_ptr<const QString>, ltShPtr > m_removeStrings;

	int m_attributeID;
};

#endif /* LOGENTRYREMOVEFILTER_H_ */
