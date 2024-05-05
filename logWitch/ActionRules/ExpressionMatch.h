/*
 * ExpressionValueGetter.h
 *
 *  Created on: Jun 5, 2011
 *      Author: sven
 */

#ifndef EXPRESSIONVALUEGETTER_H_
#define EXPRESSIONVALUEGETTER_H_
#include "Expression.h"
#include "ValueGetter.h"

class ExpressionMatch
    : public Expression
{
public:
    ExpressionMatch( TconstSharedValueGetter left );
    ExpressionMatch( TconstSharedValueGetter left, TconstSharedValueGetter right );

    void setRight( TconstSharedValueGetter right );

    bool isValid() const override;

    bool match(TconstSharedLogEntry &entry) const override;

    std::ostream &out(std::ostream &o, bool extended = false) const override;
private:
    TconstSharedValueGetter m_left;
    TconstSharedValueGetter m_right;
};


typedef std::shared_ptr<ExpressionMatch> TSharedExpressionMatch;
typedef std::shared_ptr<const ExpressionMatch> TconstSharedExpressionMatch;

#endif /* EXPRESSIONVALUEGETTER_H_ */
