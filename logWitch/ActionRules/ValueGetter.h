/*
 * ValueGetter.h
 *
 *  Created on: Jun 5, 2011
 *      Author: sven
 */

#ifndef VALUEGETTER_H_
#define VALUEGETTER_H_
#include <iostream>

#include "LogData/LogEntry.h"
#include "Types.h"

class ValueGetter
{
public:
    virtual ~ValueGetter() = default;

    virtual bool isValid( ) const = 0;

    virtual TSharedConstQString getValue( TconstSharedLogEntry &entry ) const = 0;

    virtual std::ostream &out( std::ostream &o, bool extended = false ) const = 0;
};

typedef std::shared_ptr<ValueGetter> TSharedValueGetter;
typedef std::shared_ptr<const ValueGetter> TconstSharedValueGetter;

std::ostream& operator<< (std::ostream &o, const ValueGetter &e);

#endif /* VALUEGETTER_H_ */
