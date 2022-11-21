/*
 * ExpressionParser.cpp
 *
 *  Created on: Jun 10, 2011
 *      Author: sven
 */

#include "ExpressionParser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/operator.hpp>

#include "ActionRules/ExpressionFind.h"
#include "ActionRules/ExpressionOperators.h"
#include "ActionRules/ExpressionRegEx.h"
#include "ActionRules/ExpressionMatch.h"
#include "ActionRules/ValueGetterConstQString.h"
#include "ActionRules/ValueGetterLogEntry.h"

#include "Auxiliary/BoostSpiritQStringAdaption.hxx"

using boost::spirit::tag::space;

std::ostream& operator<<(std::ostream&o, const QString& str)
{
    return o << str.toStdString();
}

std::ostream& operator<<(std::ostream&o, const TSharedExpression& str)
{
    return o << *str;
}

std::ostream& operator<<(std::ostream&o, const TSharedValueGetter& str)
{
    return o << *str;
}

namespace expressionParser
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    namespace detail
    {
        struct createVGLogEntry
        {
#if BOOST_VERSION < 105600
            template <typename S1,typename S2>
            struct result { typedef TSharedValueGetter type; };
#else
            typedef TSharedValueGetter result_type;
#endif

            TSharedValueGetter operator()(const QString &name, const TSharedConstLogEntryParserModelConfiguration configuration) const
            {
                return TSharedValueGetter( new ValueGetterLogEntry( name, configuration ) );
            }
        };
        struct createVGConstString
        {
#if BOOST_VERSION < 105600
            template <typename S1>
            struct result { typedef TSharedValueGetter type; };
#else
            typedef TSharedValueGetter result_type;
#endif

            TSharedValueGetter operator()(const QString &name) const
            {
                return TSharedValueGetter( new ValueGetterConstQString( name ) );
            }
        };
        struct constructExpVG
        {
#if BOOST_VERSION < 105600
            template <typename S1,typename S2>
            struct result { typedef TSharedExpression type; };
#else
            typedef TSharedExpression result_type;
#endif

            TSharedExpression operator()(const TSharedValueGetter &left, const TSharedValueGetter &right) const
            {
                return TSharedExpressionMatch( new ExpressionMatch( left, right ) );
            }
        };
        struct constructExpRegEx
        {
#if BOOST_VERSION < 105600
            template <typename S1,typename S2>
            struct result { typedef TSharedExpression type; };
#else
            typedef TSharedExpression result_type;
#endif

            TSharedExpression operator()(const TSharedValueGetter &value, const QString &regex) const
            {
                return TSharedExpressionRegEx( new ExpressionRegEx( value, regex ) );
            }
        };
        struct constructExpFind
        {
#if BOOST_VERSION < 105600
            template <typename S1,typename S2, typename S3>
            struct result { typedef TSharedExpression type; };
#else
            typedef TSharedExpression result_type;
#endif

            TSharedExpression operator()(const TSharedValueGetter &value, const QString &find) const
            {
                return TSharedExpressionFind( new ExpressionFind( value, find ) );
            }
        };
        struct constructExpOpNeg
        {
#if BOOST_VERSION < 105600
            template <typename S1>
            struct result { typedef TSharedExpression type; };
#else
            typedef TSharedExpression result_type;
#endif

            TSharedExpression operator()(const TSharedExpression &value) const
            {
                return TSharedExpressionOpNegate( new ExpressionOpNegate( value ) );
            }
        };
        struct constructExpOpAnd
        {
#if BOOST_VERSION < 105600
            template <typename S1,typename S2>
            struct result { typedef TSharedExpression type; };
#else
            typedef TSharedExpression result_type;
#endif

            TSharedExpression operator()(const TSharedExpression &left, const TSharedExpression &right) const
            {
                return TSharedExpressionOpAnd( new ExpressionOpAnd( left, right ) );
            }
        };
        struct constructExpOpOr
        {
#if BOOST_VERSION < 105600
            template <typename S1,typename S2>
            struct result { typedef TSharedExpression type; };
#else
            typedef TSharedExpression result_type;
#endif

            TSharedExpression operator()(const TSharedExpression &left, const TSharedExpression &right) const
            {
                return TSharedExpressionOpOr( new ExpressionOpOr( left, right ) );
            }
        };
        struct constructExpOpXOr
        {
#if BOOST_VERSION < 105600
            template <typename S1,typename S2>
            struct result { typedef TSharedExpression type; };
#else
            typedef TSharedExpression result_type;
#endif

            TSharedExpression operator()(const TSharedExpression &left, const TSharedExpression &right) const
            {
                return TSharedExpressionOpXOr( new ExpressionOpXOr( left, right ) );
            }
        };
        struct constructExpConst
        {
#if BOOST_VERSION < 105600
            template <typename S1>
            struct result { typedef TSharedExpression type; };
#else
            typedef TSharedExpression result_type;
#endif

            TSharedExpression operator()(bool value) const
            {
                return TSharedExpressionConst( new ExpressionConst( value ) );
            }
        };
    }


   template <typename Iterator>
   struct expression_grammar : qi::grammar<Iterator, TSharedExpression(), ascii::space_type>
   {
       expression_grammar( TSharedConstLogEntryParserModelConfiguration cfg) : expression_grammar::base_type(top)
       {
           using qi::lit;
           using qi::lexeme;
           using ascii::char_;
           using ascii::string;
           using namespace qi::labels;
           using boost::phoenix::function;
           using boost::phoenix::val;
           using boost::spirit::ascii::alpha;

           function<detail::createVGLogEntry> createVGLogEntry;
           function<detail::createVGConstString> createVGConstString;
           function<detail::constructExpVG> constructExpVG;
           function<detail::constructExpRegEx> constructExpRegEx;
           function<detail::constructExpOpNeg> constructExpOpNeg;
           function<detail::constructExpOpAnd> constructExpOpAnd;
           function<detail::constructExpOpOr> constructExpOpOr;
           function<detail::constructExpOpXOr> constructExpOpXOr;
           function<detail::constructExpConst> constructExpConst;
           function<detail::constructExpFind> constructExpFind;

           top =
                   expression [_val=_1]
                   | qi::eps[_val = constructExpConst(val(false))];

           expression =
                     orExprTerm [_val=_1]
                   | basicExpr [_val=_1]
                   ;

           orExprTerm =
                     ((xorExprTerm >> "||" >> xorExprTerm)[_val=constructExpOpOr(_1,_2)])
                   | xorExprTerm [_val=_1]
                   ;

           xorExprTerm =
                     ((andExprTerm >> "^^" >> andExprTerm)[_val = constructExpOpXOr(_1,_2)])
                   | andExprTerm [_val=_1]
                   ;

           andExprTerm =
                     ((unaryExprTerm >> "&&" >> unaryExprTerm)[_val = constructExpOpAnd(_1,_2)])
                   | unaryExprTerm [_val=_1]
                   ;

           unaryExprTerm =
                     ("!(" >> expression[_val = constructExpOpNeg(_1)] >> ")" )
                   | ('(' >> expression >> ')') [_val=_1]
                   | basicExpr [_val=_1]
                   ;

           basicExpr =
                     expressionVG [_val=_1]
                   | expressionRegEx [_val=_1]
                   | expressionFind [_val=_1]
                   | expressionConst [_val=_1]
                   ;

           expressionVG = ( valueGetter >> "==" >> valueGetter)[ _val = constructExpVG(_1, _2 )]
                     | ( valueGetter >> "!=" >> valueGetter)[ _val = constructExpVG(_1, _2 )]
                         >> qi::eps[_val = constructExpOpNeg(_val)] ;

           expressionRegEx = (valueGetter >> "=~" >> quotedQStringRegEx)[ _val = constructExpRegEx(_1, _2)]
                     | (valueGetter >> "!~" >> quotedQStringRegEx)[ _val = constructExpRegEx(_1, _2)]
                         >> qi::eps[_val = constructExpOpNeg(_val)] ;

           expressionConst =
                     lit("true") [_val = constructExpConst(val(true))]
                   | lit("false") [_val = constructExpConst(val(false))]
                   ;

           expressionFind = ("find(" >> valueGetter >> "," >> quotedQString('"') >> ")")[ _val = constructExpFind(_1, _2)];

           valueGetter %= vg_ConstString
                   | vg_LogEntry;

           vg_ConstString = quotedQString('"')[_val = createVGConstString(_1)];
           vg_LogEntry = unquotedQString[_val = createVGLogEntry(_1,cfg)];

           quotedQString = lexeme[
                                  lit(_r1)
                               >> +(
                                       ( '\\' >> char_(_r1) ) [_val += _1]
                                       || (char_ - lit(_r1) ) [_val += _1]
                                   )
                               >> lit(_r1)];

           quotedQStringRegEx %=
                     quotedQString('"')
                   | quotedQString('/')
                   | quotedQString('|')
                   | quotedQString('#')
                   ;

           unquotedQString %=  +(alpha);

           BOOST_SPIRIT_DEBUG_NODE(top);
           BOOST_SPIRIT_DEBUG_NODE(expression);
           BOOST_SPIRIT_DEBUG_NODE(orExprTerm);
           BOOST_SPIRIT_DEBUG_NODE(xorExprTerm);
           BOOST_SPIRIT_DEBUG_NODE(andExprTerm);
           BOOST_SPIRIT_DEBUG_NODE(unaryExprTerm);
           BOOST_SPIRIT_DEBUG_NODE(basicExpr);

           BOOST_SPIRIT_DEBUG_NODE(expressionVG);
           BOOST_SPIRIT_DEBUG_NODE(expressionFind);
           BOOST_SPIRIT_DEBUG_NODE(expressionRegEx);
           BOOST_SPIRIT_DEBUG_NODE(expressionConst);

           BOOST_SPIRIT_DEBUG_NODE(valueGetter);
           BOOST_SPIRIT_DEBUG_NODE(vg_LogEntry);
           BOOST_SPIRIT_DEBUG_NODE(vg_ConstString);

           BOOST_SPIRIT_DEBUG_NODE(quotedQString);
           BOOST_SPIRIT_DEBUG_NODE(unquotedQString);
       }

       qi::rule<Iterator, TSharedExpression(), ascii::space_type> top;
       qi::rule<Iterator, TSharedExpression(), ascii::space_type> expression;
       qi::rule<Iterator, TSharedExpression(), ascii::space_type> orExprTerm;
       qi::rule<Iterator, TSharedExpression(), ascii::space_type> xorExprTerm;
       qi::rule<Iterator, TSharedExpression(), ascii::space_type> andExprTerm;
       qi::rule<Iterator, TSharedExpression(), ascii::space_type> unaryExprTerm;
       qi::rule<Iterator, TSharedExpression(), ascii::space_type> basicExpr;
       qi::rule<Iterator, TSharedExpression(), ascii::space_type> expressionVG;
       qi::rule<Iterator, TSharedExpression(), ascii::space_type> expressionFind;
       qi::rule<Iterator, TSharedExpression(), ascii::space_type> expressionRegEx;
       qi::rule<Iterator, TSharedExpression(), ascii::space_type> expressionConst;

       qi::rule<Iterator, TSharedValueGetter(), ascii::space_type> valueGetter;
       qi::rule<Iterator, TSharedValueGetter(), ascii::space_type> vg_LogEntry;
       qi::rule<Iterator, TSharedValueGetter(), ascii::space_type> vg_ConstString;
       qi::rule<Iterator,  QString(const char), ascii::space_type> quotedQString;
       qi::rule<Iterator,  QString(), ascii::space_type> quotedQStringRegEx;
       qi::rule<Iterator,  QString(), ascii::space_type> unquotedQString;
   };
}

ExpressionParser::ExpressionParser( TSharedConstLogEntryParserModelConfiguration cfg )
    : m_cfg( cfg )
{

}

bool ExpressionParser::parse(  const QString &expression )
{
    std::string str = expression.toStdString();

    typedef expressionParser::expression_grammar<std::string::const_iterator> exp_grammar;
    exp_grammar expGram( m_cfg ); // Our grammar

    using boost::spirit::ascii::space;
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    bool r = phrase_parse(iter, end, expGram, space, m_expression);

    if (r && iter == end)
    {
#ifndef NDEBUG
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << *m_expression << std::endl;
        std::cout << "-------------------------\n";
        std::cout << "- Extended: -\n";
        m_expression->out( std::cout, true );
        std::cout << "-------------------------\n";
#endif
        m_error = "";
    }
    else
    {
        m_expression = TSharedExpression();
        std::string::const_iterator some = iter+30;
        std::string context(iter, (some>end)?end:some);
#ifndef NDEBUG
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "stopped at: \": " << context << "...\"\n";
        std::cout << "-------------------------\n";
#endif
        m_error = QString::fromStdString( "Parsing failed near: " + context );
    }

    return isValid();
}

const QString &ExpressionParser::getError() const
{
    return m_error;
}

TSharedExpression ExpressionParser::get() const
{
    return m_expression;
}

bool ExpressionParser::isValid() const
{
    return (bool)m_expression;
}

ExpressionParser::~ExpressionParser()
{
}
