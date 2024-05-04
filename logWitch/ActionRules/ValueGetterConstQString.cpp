/*
 * ValueGetterConstQString.cpp
 *
 *  Created on: Jun 5, 2011
 *      Author: sven
 */

#include "ValueGetterConstQString.h"

ValueGetterConstQString::ValueGetterConstQString( const QString &string)
    : m_string( new QString( string ) )
{

}

ValueGetterConstQString::ValueGetterConstQString( TSharedConstQString &string )
    : m_string( string )
{

}

TSharedConstQString ValueGetterConstQString::getValue( TconstSharedLogEntry & ) const
{
    return m_string;
}

std::ostream &ValueGetterConstQString::out( std::ostream &o, bool extended ) const
{
    QString str = *m_string;
    str.replace('"',"\\\"");
    if( extended )
    {
        o << "ValueGetterConstQString{"<< "\"" << str.toStdString() << "\"" << "}";
        return o;
    }
    else
    {
        o << "\"" << str.toStdString() << "\"";
        return o;
    }
}
