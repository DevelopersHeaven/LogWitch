/*
 * Rule.h
 *
 *  Created on: Jun 5, 2011
 *      Author: sven
 */

#ifndef RULE_H_
#define RULE_H_

#include <QtCore/QtCore>

#include "ActionRules/Action.h"
#include "ActionRules/Expression.h"

#include "LogData/LogEntry.h"
/**
 * A Rule describes what to do with a log entry when this is true.
 */
class Rule
{
public:
    Rule( TconstSharedExpression expr, TconstSharedAction action );
    virtual ~Rule();

    bool checkRule( TconstSharedLogEntry &entry ) const;
    TconstSharedAction getAction( ) const;
    TconstSharedExpression getExpression( ) const;

    bool isValid() const;

private:
    TconstSharedExpression m_expr;
    TconstSharedAction m_action;
};

typedef std::shared_ptr<Rule> TSharedRule;
typedef std::shared_ptr<const Rule> TconstSharedRule;

#endif /* RULE_H_ */
