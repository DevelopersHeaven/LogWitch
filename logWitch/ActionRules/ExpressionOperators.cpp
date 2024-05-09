/*
 * ExpressionOperators.cpp
 *
 *  Created on: Jun 25, 2011
 *      Author: sven
 */

#include "ExpressionOperators.h"

#include <iostream>
#include <utility>

ExpressionOpNegate::ExpressionOpNegate( TSharedExpression in )
: m_expr(std::move( in ))
{
}

bool ExpressionOpNegate::isValid( ) const
{
    return m_expr && m_expr->isValid();
}

bool ExpressionOpNegate::match( TconstSharedLogEntry &entry ) const
{
    return !m_expr->match( entry );
}

std::ostream &ExpressionOpNegate::out( std::ostream &o, bool extended ) const
{
    o << "!(";
    m_expr->out( o, extended );
    o << ")";
    return o;
}

ExpressionConst::ExpressionConst( bool v )
: m_value( v )
{
}

bool ExpressionConst::isValid( ) const
{
    return true;
}

bool ExpressionConst::match( TconstSharedLogEntry & ) const
{
    return m_value;
}

std::ostream &ExpressionConst::out( std::ostream &o, bool  ) const
{
    if( m_value )
        o << "true";
    else
        o << "false";

    return o;
}

ExpressionOpOr::ExpressionOpOr( TSharedExpression left, TSharedExpression right )
: m_left(std::move( left ))
, m_right(std::move( right ))
{
}

bool ExpressionOpOr::isValid( ) const
{
    return m_left && m_left->isValid() && m_right && m_right->isValid();
}

bool ExpressionOpOr::match( TconstSharedLogEntry &entry ) const
{
    return m_left->match( entry ) || m_right->match(entry);
}

std::ostream &ExpressionOpOr::out( std::ostream &o, bool extended ) const
{
    o << "(";
    m_left->out( o, extended );
    o << "||";
    m_right->out( o, extended );
    o << ")";
    return o;
}

ExpressionOpAnd::ExpressionOpAnd( TSharedExpression left, TSharedExpression right )
: m_left(std::move( left ))
, m_right(std::move( right ))
{
}

bool ExpressionOpAnd::isValid( ) const
{
    return m_left && m_left->isValid() && m_right && m_right->isValid();
}

bool ExpressionOpAnd::match( TconstSharedLogEntry &entry ) const
{
    return m_left->match( entry ) && m_right->match(entry);
}

std::ostream &ExpressionOpAnd::out( std::ostream &o, bool extended ) const
{
    o << "(";
    m_left->out( o, extended );
    o << "&&";
    m_right->out( o, extended );
    o << ")";
    return o;
}

ExpressionOpXOr::ExpressionOpXOr( TSharedExpression left, TSharedExpression right )
: m_left(std::move( left ))
, m_right(std::move( right ))
{
}

bool ExpressionOpXOr::isValid( ) const
{
    return m_left && m_left->isValid() && m_right && m_right->isValid();
}

bool ExpressionOpXOr::match( TconstSharedLogEntry &entry ) const
{
    return m_left->match( entry ) ^ m_right->match(entry);
}

std::ostream &ExpressionOpXOr::out( std::ostream &o, bool extended ) const
{
    o << "(";
    m_left->out( o, extended );
    o << "^^";
    m_right->out( o, extended );
    o << ")";
    return o;
}
