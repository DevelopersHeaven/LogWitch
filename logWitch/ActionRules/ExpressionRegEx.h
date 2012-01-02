/*
 * ExpressionRegEx.h
 *
 *  Created on: Jun 25, 2011
 *      Author: sven
 */

#ifndef EXPRESSIONREGEX_H_
#define EXPRESSIONREGEX_H_


#include "ActionRules/Expression.h"
#include <iostream>
#include "ValueGetter.h"
#include <QRegExp>

class ExpressionRegEx
    : public Expression
{
public:
    ExpressionRegEx( TconstSharedValueGetter value, const QString &exp);
    ~ExpressionRegEx();

    bool isValid( ) const;

    bool match( TconstSharedLogEntry &entry ) const;

    std::ostream &out( std::ostream &o, bool extended = false ) const;
private:
    TconstSharedValueGetter m_value;

    QRegExp m_regex;
};

typedef boost::shared_ptr<ExpressionRegEx> TSharedExpressionRegEx;
typedef boost::shared_ptr<const ExpressionRegEx> TconstSharedExpressionRegEx;


#endif /* EXPRESSIONREGEX_H_ */
