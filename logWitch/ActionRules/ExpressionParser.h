/*
 * ExpressionParser.h
 *
 *  Created on: Jun 10, 2011
 *      Author: sven
 */

#ifndef EXPRESSIONPARSER_H_
#define EXPRESSIONPARSER_H_
#include <QString>

#include "ActionRules/Expression.h"
#include "LogData/LogEntryParserModelConfiguration.h"

class ExpressionParser
{
public:
    ExpressionParser( TSharedConstLogEntryParserModelConfiguration cfg = TSharedConstLogEntryParserModelConfiguration() );

    /**
     * Parse an expression and returns true if successful.
     */
    bool parse(  const QString &expression );

    /**
     * Returns the last parsed Expression
     */
    TSharedExpression get() const;

    /**
     * Returns true if the last expression was parsed successful.
     */
    bool isValid() const;

    const QString &getError() const;

private:
    TSharedExpression m_expression;

    TSharedConstLogEntryParserModelConfiguration m_cfg;

    QString m_error;
};

#endif /* EXPRESSIONPARSER_H_ */
