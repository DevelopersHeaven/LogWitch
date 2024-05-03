/*
 * ActionParser.cpp
 *
 *  Created on: Jun 19, 2011
 *      Author: sven
 */

#include "ActionParser.h"

#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/fusion.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <QtGui>

#include "ActionRules/ActionDataRewriter.h"
#include "ActionRules/ActionDoNothing.h"
#include "ActionRules/ActionDiscardRow.h"

#include "Auxiliary/BoostSpiritQStringAdaption.hxx"

using boost::spirit::locals;

namespace actionParser
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    typedef std::pair<int, QVariant> TRoleVariantPair;
    typedef std::pair<QString, TRoleVariantPair> TColumnRoleVariantPair;

    namespace detail
    {
        struct addRewriteRule
        {
#if BOOST_VERSION < 105600
            template <typename S1, typename S2>
            struct result { typedef void type; };
#else
            typedef void result_type;
#endif

            void operator()(TSharedActionDataRewriter& entry, const TRoleVariantPair & rule ) const
            {
                entry->addChangeSet( rule.second, rule.first );
            }

            void operator()(TSharedActionDataRewriter& entry, const TColumnRoleVariantPair & rule ) const
            {
                entry->addChangeSet( rule.second.second, rule.second.first, rule.first );
            }
        };
    }

    struct qColorSymb_ : qi::symbols<char, QColor>
    {
        qColorSymb_()
        {
            add
            ("white"        , QColor(Qt::white) )
            ("red"          , QColor(Qt::red) )
            ("green"        , QColor(Qt::green) )
            ("blue"         , QColor(Qt::blue) )
            ("black"        , QColor(Qt::black) )
            ("darkRed"      , QColor(Qt::darkRed) )
            ("darkGreen"    , QColor(Qt::darkGreen) )
            ("darkBlue"     , QColor(Qt::darkBlue) )
            ("cyan"         , QColor(Qt::cyan) )
            ("magenta"      , QColor(Qt::magenta) )
            ("yellow"       , QColor(Qt::yellow) )
            ("gray"         , QColor(Qt::gray) )
            ("darkCyan"     , QColor(Qt::darkCyan) )
            ("darkMagenta"  , QColor(Qt::darkMagenta) )
            ("darkYellow"   , QColor(Qt::darkYellow) )
            ("darkGray"     , QColor(Qt::darkGray) )
            ("lightGray"    , QColor(Qt::lightGray) )
            ;
        }

    } qColorSymb;

    struct qIconSymb_ : qi::symbols<char, QVariant>
    {
        qIconSymb_()
        {
            add
            ("fatal"        , QIcon(":/icons/fatal") )
            ("error"        , QIcon(":/icons/error") )
            ("warn"         , QIcon(":/icons/warning") )
            ("info"         , QIcon(":/icons/information") )
            ("trace"        , QIcon(":/icons/trace") )
            ;
        }
    };

    struct qColorRoles_ : qi::symbols<char, int>
    {
        qColorRoles_()
        {
            add
            ("FG"       , Qt::ForegroundRole )
            ("BG"       , Qt::BackgroundRole )
            ;
        }

    } qColorRoles;


    template <typename Iterator>
    struct action_grammar : qi::grammar<Iterator, TSharedAction(), ascii::space_type>
    {
        action_grammar( TSharedConstLogEntryParserModelConfiguration cfg ) : action_grammar::base_type(action)
        {
            using qi::lit;
            using qi::lexeme;
            using qi::unused_type;
            using ascii::char_;
            using ascii::string;
            using namespace qi::labels;
            using boost::phoenix::function;
            using boost::phoenix::val;
            using boost::phoenix::ref;
            using boost::phoenix::at_c;
            using boost::phoenix::construct;
            using boost::phoenix::new_;
            using boost::spirit::ascii::alpha;
            using boost::spirit::qi::uint_parser;
            using boost::spirit::qi::locals;

            function<detail::addRewriteRule> addRewriteRule;

            action = actionDataRewriter [_val = _1]
                    | actionDiscardRow [_val = _1]
                    | qi::eps [ _val = construct<TSharedAction>( new_<ActionDoNothing>( ) ) ];

            actionDataRewriter =
                    qi::eps [_val = construct<TSharedActionDataRewriter>( new_<ActionDataRewriter>( cfg ) )]
                    >> "rewrite("
                    >>  ( rewriteRule [ addRewriteRule(_val, _1)]
                           | rewriteRuleColumn [ addRewriteRule(_val, _1)]
                        ) % ','
                    >> ')' ;

            actionDiscardRow =
                    lit("discard()") [_val = construct<TSharedAction>( new_<ActionDiscardRow>() )];

           rewriteRule %= rewriteRuleColor
                   | rewriteRuleIcon
                   | rewriteRuleText;

           rewriteRuleColor = qColorRoles[at_c<0>(_val)=_1] >> ':' >> ruleQColor[at_c<1>(_val)=_1];

           uint_parser<unsigned, 16, 2, 2> hex2_2_p;

           ruleQColor = qColorSymb [_val = _1]
                   | '#' >> hex2_2_p [_a = _1]
                         >> hex2_2_p [_b = _1]
                         >> hex2_2_p [_val = construct<QColor>(_a,_b,_1) ];

           rewriteRuleIcon = lit("icon") [at_c<0>(_val) = val(Qt::DecorationRole)] >> ':' >> qIconSymb [at_c<1>(_val) = _1];

           rewriteRuleText = lit("text") >> ':' >> qi::eps [ at_c<1>(_val) = val(QString())];

           rewriteRuleColumn = unquotedQString  >> ':' >> rewriteRule;

           //quotedQString %= lexeme['"' >> +(char_ - '"') >> '"'];
           unquotedQString %=  +(alpha) ;

        }

        // qi::rule<Iterator,  QString(), ascii::space_type> quotedQString;
        qi::rule<Iterator,  QString(), ascii::space_type> unquotedQString;
        qi::rule<Iterator,  QColor(), ascii::space_type, boost::spirit::locals<unsigned int, unsigned int> > ruleQColor;

        qi::rule<Iterator, TSharedAction(), ascii::space_type> action;
        qi::rule<Iterator, TSharedActionDataRewriter(), ascii::space_type> actionDataRewriter;
        qi::rule<Iterator, TSharedAction(), ascii::space_type> actionDiscardRow;

        qi::rule<Iterator, TRoleVariantPair(), ascii::space_type> rewriteRule;
        qi::rule<Iterator, TRoleVariantPair(), ascii::space_type> rewriteRuleColor;
        qi::rule<Iterator, TRoleVariantPair(), ascii::space_type> rewriteRuleIcon;
        qi::rule<Iterator, TRoleVariantPair(), ascii::space_type> rewriteRuleText;
        qi::rule<Iterator, TColumnRoleVariantPair(), ascii::space_type> rewriteRuleColumn;

        // qi::rule<Iterator, QColor(), ascii::space_type> color;
        struct qIconSymb_ qIconSymb;
    };
}


ActionParser::ActionParser( TSharedConstLogEntryParserModelConfiguration cfg )
: m_cfg( cfg )
{
}

ActionParser::~ActionParser()
{
}


bool ActionParser::parse(  const QString &expression )
{
    std::string str = expression.toStdString();
    typedef actionParser::action_grammar<std::string::const_iterator> exp_grammar;
    exp_grammar expGram( m_cfg ); // Our grammar

    using boost::spirit::ascii::space;
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    bool r = phrase_parse(iter, end, expGram, space, m_action);

    if (r && iter == end)
    {
//        std::cout << "-------------------------\n";
//        std::cout << "Parsing succeeded\n";
//        // std::cout << *m_action << std::endl;
//        std::cout << "-------------------------\n";
//        std::cout << "- Extended: -\n";
//        // m_action->out( std::cout, true );
//        std::cout << "-------------------------\n";
        m_error = "";
    }
    else
    {
        m_action = TSharedAction();
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

const QString &ActionParser::getError() const
{
    return m_error;
}

TSharedAction ActionParser::get() const
{
    return m_action;
}

bool ActionParser::isValid() const
{
    return m_error.length() == 0;
}
