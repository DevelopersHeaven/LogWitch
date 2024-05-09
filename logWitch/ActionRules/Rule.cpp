/*
 * Rule.cpp
 *
 *  Created on: Jun 5, 2011
 *      Author: sven
 */

#include "Rule.h"

#include <utility>

Rule::Rule( TconstSharedExpression expr, TconstSharedAction action )
    : m_expr(std::move( expr ))
    , m_action(std::move( action ))
{
}

bool Rule::isValid() const
{
    return m_expr && m_expr->isValid() && m_action && m_action->isValid();
}

bool Rule::checkRule( TconstSharedLogEntry &entry ) const
{
    return m_expr->match( entry );
}

TconstSharedAction Rule::getAction( ) const
{
    return m_action;
}

TconstSharedExpression Rule::getExpression( ) const
{
    return m_expr;
}
