/*
 * FilterRuleCompiled.cpp
 *
 *  Created on: Jun 10, 2011
 *      Author: sven
 */

#include "FilterRuleCompiled.h"

#include <boost/phoenix/operator.hpp>
#include <boost/spirit/include/qi.hpp>

// take the string and search for => as an special expression.
static const QString seperator("=>");

namespace ruleParser
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    struct ruleParts
    {
        QString description;
        QString expression;
        QString action;
    };
}

BOOST_FUSION_ADAPT_STRUCT(
    ruleParser::ruleParts,
    (QString, description)
    (QString, expression)
    (QString, action)
)

namespace ruleParser
{
    template <typename Iterator>
    struct ruleParts_parser : qi::grammar<Iterator, ruleParts(), ascii::space_type>
    {
        ruleParts_parser() : ruleParts_parser::base_type(top)
        {
            using qi::lit;
            using qi::lexeme;
            using ascii::char_;
            using namespace qi::labels;
            using boost::phoenix::val;

            // Example: "Description":{Expression}=>{Action}
            top = quotedQString('"','"') >> lit(":") >> quotedQString('{','}') >> lit( seperator.toStdString() ) >> quotedQString('{','}');

            quotedQString = lexeme[
                                   lit(_r1)
                                   >> *(
                                           ( '\\' >> char_ )      [_val += _1]
                                        || (char_ - lit(_r2) )    [_val += _1]
                                   )
                                   >> lit(_r2)];
        }

        qi::rule<Iterator, ruleParts(), ascii::space_type> top;
        qi::rule<Iterator, QString(const char,const char), ascii::space_type> quotedQString;
    };
}

FilterRuleCompiled::FilterRuleCompiled( const QString &rule, TSharedConstLogEntryParserModelConfiguration cfg )
: m_expression( cfg )
, m_action( cfg )
{
    std::string str = rule.toStdString();
    ruleParser::ruleParts parts;

    typedef ruleParser::ruleParts_parser<std::string::const_iterator> rule_grammar;
    rule_grammar ruleGrammar; // Our grammar

    using boost::spirit::ascii::space;
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    bool r = phrase_parse(iter, end, ruleGrammar, space, parts);

    if (r && iter == end)
    {
        setUserDescription( parts.description );
        expressionString( parts.expression );
        actionString( parts.action );
    }
    else
    {
        std::string::const_iterator some = iter+30;
        std::string context(iter, (some>end)?end:some);
        qDebug() << "-------------------------";
        qDebug() << "Rule extraction failed";
        qDebug() << "stopped at: "<< QString::fromStdString(context) << "...";
        qDebug() << "Complete rule: " << rule;
        qDebug() << "-------------------------";
    }

    parseRule();
}

FilterRuleCompiled::~FilterRuleCompiled()
{
}

TSharedExpression FilterRuleCompiled::getExpression() const
{
    return m_expression.get();
}

bool FilterRuleCompiled::validWithinContext() const
{
    return m_expression.isValid() && m_expression.get()->isValid()
            && m_action.isValid() && m_action.get()->isValid();
}

void FilterRuleCompiled::parseRule()
{
    m_action.parse( m_actionAsString );
    m_expression.parse( m_expressionAsString );
    if( m_expression.isValid() && m_action.isValid() )
    {
        m_compiledRule.reset( new Rule( m_expression.get(), m_action.get() ) );
    }
    else
        m_compiledRule.reset();

    emit changed();
}

TSharedRule FilterRuleCompiled::getCompiledRule()
{
    return m_compiledRule;
}

QString FilterRuleCompiled::toString() const
{
    QString quotedDescription = getUserDescription();
    quotedDescription.replace('\\',"\\\\").replace( '\"', "\\\"");

    QString quotedExpression = expressionString();
    quotedExpression.replace('\\',"\\\\").replace( '}', "\\}");

    QString quotedAction = actionString();
    quotedAction.replace('\\',"\\\\").replace( '}', "\\}");

    return "\"" + quotedDescription + "\":{" + quotedExpression + "}" + seperator + "{" + quotedAction + "}";
}

void FilterRuleCompiled::expressionString( const QString &exp )
{
    m_expressionAsString = exp;
    m_expression.parse( m_expressionAsString );
    parseRule();
}

const QString &FilterRuleCompiled::expressionString( ) const
{
    return m_expressionAsString;
}

const QString &FilterRuleCompiled::getExpressionError() const
{
    return m_expression.getError();
}

bool FilterRuleCompiled::isExpressionOk() const
{
    return m_expression.isValid();
}

void FilterRuleCompiled::actionString( const QString &act)
{
    m_actionAsString = act;
    m_action.parse( m_actionAsString);
    parseRule();
}

TconstSharedDisplayItemData FilterRuleCompiled::getActionDisplayer() const
{
    return m_action.get();
}

const QString &FilterRuleCompiled::actionString() const
{
    return m_actionAsString;
}


bool FilterRuleCompiled::isActionOk() const
{
    return m_action.isValid();
}


const QString &FilterRuleCompiled::getActionError() const
{
    return m_action.getError();
}

const QString &FilterRuleCompiled::getUserDescription() const
{
    return m_userDescription;
}

void FilterRuleCompiled::setUserDescription( const QString &desc )
{
    m_userDescription = desc;
}
