/*
 * ValueGetterConstQString.h
 *
 *  Created on: Jun 5, 2011
 *      Author: sven
 */

#ifndef VALUEGETTERCONSTQSTRING_H_
#define VALUEGETTERCONSTQSTRING_H_
#include "ValueGetter.h"

class ValueGetterConstQString
    :public ValueGetter
{
public:
    ValueGetterConstQString( const QString &string );
    ValueGetterConstQString( TSharedConstQString &string );

    bool isValid( ) const { return true; };

    virtual TSharedConstQString getValue( TconstSharedLogEntry &entry ) const;

    std::ostream &out( std::ostream &o, bool extended = false ) const;

private:
    TSharedConstQString m_string;
};

#endif /* VALUEGETTERCONSTQSTRING_H_ */
