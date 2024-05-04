/*
 * ExpressionRegEx.cpp
 *
 *  Created on: Jun 25, 2011
 *      Author: sven
 */

#include "ExpressionRegEx.h"
#include "Types.h"

ExpressionRegEx::ExpressionRegEx(TconstSharedValueGetter value, const QString &exp)
: m_value(value)
, m_regex( exp )
{
}

ExpressionRegEx::~ExpressionRegEx()
{
}

bool ExpressionRegEx::isValid( ) const
{
    return m_value && m_value->isValid() && m_regex.isValid();
}

bool ExpressionRegEx::match( TconstSharedLogEntry &entry ) const
{
    TSharedConstQString value = m_value->getValue( entry );
    return value->contains( m_regex );
}

QString ExpressionRegEx::getPattern() const
{
  return m_regex.pattern();
}

TconstSharedValueGetter ExpressionRegEx::getValueGetter() const
{
  return m_value;
}

std::ostream &ExpressionRegEx::out( std::ostream &o, bool extended ) const
{
    if( extended )
    {
        o << "ExpressionRegEx{";
        m_value->out( o, extended );
        o << " =~ \"";
        o << m_regex.pattern().toStdString();
        o << "\"}";
    }
    else
    {
        o << *m_value << " =~ \"" << m_regex.pattern().toStdString() << "\"";
    }

    return o;
}

