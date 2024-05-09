/*
 * ExpressionValueGetter.cpp
 *
 *  Created on: Jun 5, 2011
 *      Author: sven
 */

#include "ExpressionMatch.h"

#include "ValueGetter.h"
#include <utility>

ExpressionMatch::ExpressionMatch( TconstSharedValueGetter left, TconstSharedValueGetter right )
    : m_left(std::move( left ))
    , m_right(std::move( right ))
{
}

ExpressionMatch::ExpressionMatch( TconstSharedValueGetter left )
    : m_left(std::move( left ))
    , m_right(  )
{
}

void ExpressionMatch::setRight( TconstSharedValueGetter right )
{
    m_right = right;
}

bool ExpressionMatch::match( TconstSharedLogEntry &entry ) const
{
    return *(m_left->getValue( entry )) == *(m_right->getValue( entry ));
}

bool ExpressionMatch::isValid( ) const
{
    return m_left && m_left->isValid() && m_right && m_right->isValid();
}

std::ostream &ExpressionMatch::out( std::ostream &o, bool extended ) const
{
    if( extended )
    {
        o << "ExprVG{";
        m_left->out( o, extended );
        o << " == ";
        m_right->out( o, extended );
        o << "}";
    }
    else
    {
        o << *m_left << " == " << *m_right;
    }

    return o;
}
