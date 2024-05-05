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

    bool isValid() const override { return true; }

    TSharedConstQString getValue(TconstSharedLogEntry &entry) const override;

    std::ostream &out(std::ostream &o, bool extended = false) const override;

private:
    TSharedConstQString m_string;
};

#endif /* VALUEGETTERCONSTQSTRING_H_ */
