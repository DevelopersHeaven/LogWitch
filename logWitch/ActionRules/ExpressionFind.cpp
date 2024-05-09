/*
 * ExpressionFind.cpp
 *
 *  Created on: Dec 3, 2011
 *      Author: sven
 */

#include "ExpressionFind.h"

#include <utility>

#include "Types.h"

ExpressionFind::ExpressionFind(TconstSharedValueGetter value, QString exp)
: m_value(std::move(value))
, m_pattern(std::move( exp ))
{
}

bool ExpressionFind::isValid( ) const
{
    return m_value && m_value->isValid();
}

bool ExpressionFind::match( TconstSharedLogEntry &entry ) const
{
    TSharedConstQString value = m_value->getValue( entry );
    return value->contains( m_pattern );
}

QString ExpressionFind::getPattern() const
{
  return m_pattern;
}

TconstSharedValueGetter ExpressionFind::getValueGetter() const
{
  return m_value;
}

std::ostream &ExpressionFind::out( std::ostream &o, bool extended ) const
{
    if( extended )
    {
        o << "ExpressionFind{";
        m_value->out( o, extended );
        o << " =~ \"";
        o << m_pattern.toStdString();
        o << "\"}";
    }
    else
    {
        o << "find("<<*m_value << ",\"" << m_pattern.toStdString() << "\")";
    }

    return o;
}

