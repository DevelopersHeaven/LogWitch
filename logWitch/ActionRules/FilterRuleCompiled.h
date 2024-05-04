/*
 * FilterRuleCompiled.h
 *
 *  Created on: Jun 10, 2011
 *      Author: sven
 */
#ifndef FILTERRULECOMPILED_H_
#define FILTERRULECOMPILED_H_

#include "ActionRules/Action.h"
#include "ActionRules/ActionParser.h"
#include "ActionRules/Expression.h"
#include "ActionRules/ExpressionParser.h"
#include "ActionRules/Rule.h"

#include "LogData/LogEntryParserModelConfiguration.h"

class FilterRuleCompiled;
typedef std::shared_ptr<FilterRuleCompiled> TSharedFilterRuleCompiled;
typedef std::shared_ptr<const FilterRuleCompiled> TSharedConstFilterRuleCompiled;

class FilterRuleCompiled
: public QObject
{
    Q_OBJECT
public:
    FilterRuleCompiled( const QString &str, TSharedConstLogEntryParserModelConfiguration cfg = TSharedConstLogEntryParserModelConfiguration() );

    /**
     * Sets an expression string and parses him.
     */
    void expressionString( const QString &exp );

    /**
     * Retrieves the current expresseion string.
     */
    const QString &expressionString( ) const;

    /**
     * Returns true if the current set expression is compilable.
     */
    bool isExpressionOk() const;

    /**
     * Returns the error from the parser if isExpressionOk returns false.
     */
    const QString &getExpressionError() const;

    /**
     * Sets a new string for an action.
     */
    void actionString( const QString &act);

    /**
     * Retrieve the string for the action.
     */
    const QString &actionString() const;

    /**
     * Returns true if actionString is parseable to an Action object.
     */
    bool isActionOk() const;

    /**
     * If isActionOk is false, this function retrieves the error.
     */
    const QString &getActionError() const;

    /**
     * This holds an action drawer which knows how to draw the current
     * actionAsString in a more human readable way.
     */
    TconstSharedDisplayItemData getActionDisplayer() const;

    /**
     * Returns a full description of the rule, which can be used to recreate the
     * basic rule.
     */
    QString toString() const;

    /**
     *  If the expression is valid within the current
     * context, getExpression returns the expression.
     */
    TSharedExpression getExpression() const;

    /**
     * Returns the action compiled within this context.
     */
    TSharedAction getAction() const;

    /**
     * Return true if the rule is compilable and valid within the current context.
     */
    bool validWithinContext() const;

    /**
     * Returns the compiled rules. This rule is ready for using inside the filter.
     * The compiled rule may be empty if the expression or the action is invalid.
     */
    TSharedRule getCompiledRule();

    /**
     * Returns the user description of this rule.
     */
    const QString &getUserDescription() const;

    /**
     * Sets the user description.
     */
    void setUserDescription( const QString &desc );

signals:
    void changed();

protected slots:
    void parseRule();

private:
    /// This is the expression in the common string notation.
    QString m_expressionAsString;

    /// This is the action in the common string notation.
    QString m_actionAsString;

    /// The expression parser with the parsed expression if any.
    ExpressionParser m_expression;

    /// The action parser with the parsed action if any.
    ActionParser m_action;

    /// This is the compiled rule which is used for filtering.
    TSharedRule m_compiledRule;

    /**
     * This description is a note for the user to identify its rule.
     */
    QString m_userDescription;
};

#endif /* FILTERRULECOMPILED_H_ */
