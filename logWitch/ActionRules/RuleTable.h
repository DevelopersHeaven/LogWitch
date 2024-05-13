/*
 * RuleTable.h
 *
 *  Created on: Jun 5, 2011
 *      Author: sven
 */

#ifndef RULETABLE_H_
#define RULETABLE_H_
#include <list>
#include <set>
#include <map>
#include <string>

#include <QtCore/qobject.h>

#include "Rule.h"

class RuleTable;

typedef std::shared_ptr<RuleTable> TSharedRuleTable;
typedef std::shared_ptr<const RuleTable> TconstSharedRuleTable;

class RuleTable
    :public QObject
{
    Q_OBJECT
public:
    typedef std::list< TconstSharedRule > TRuleList;
    typedef std::set< TconstSharedRule > TRuleSet;
    typedef std::map<std::string,TRuleSet> TRuleTableMap;

//    template <class T>
//    TSharedRuleTable filterRulesByType( ) const
//    {
//        TSharedRuleTable table( new RuleTable );
//
//        TRuleList::iterator it;
//        for( it = m_rules.begin(); it != m_rules.end(); ++it )
//        {
//            std::shared_ptr<T> rule = std::dynamic_pointer_cast<T>( *it );
//            if( rule )
//                table->m_rules.push_back( rule );
//        }
//        return table;
//    }

    /**
     * Adds a new table with a new unique name. This unique name will be
     * returned to the caller.
     */
    std::string addNewUniqueTable();

    void addRule( const std::string& tableName, TSharedRule &rule );

    void clear( const std::string& tableName );

    // TActionList getActionsForEntry( TconstSharedLogEntry &entry ) const;
    template <class T>
    bool getMatchedActionsForType( std::list<T> &actions, TconstSharedLogEntry &entry ) const
    {
        TRuleSet::const_iterator it;
        for( it = m_rules.begin(); it != m_rules.end(); ++it )
        {
            T action = std::dynamic_pointer_cast<typename T::element_type>( (*it)->getAction() );
            if( action && (*it)->checkRule( entry ) )
                actions.push_back( action );
        }
        return !actions.empty();
    }

    template <class T>
    bool isActionMatched( TconstSharedLogEntry &entry ) const
    {
        TRuleSet::const_iterator it;
        for( it = m_rules.begin(); it != m_rules.end(); ++it )
        {
            T action = std::dynamic_pointer_cast<typename T::element_type>( (*it)->getAction() );
            if( action && (*it)->checkRule( entry ) )
                return true;
        }
        return false;
    }

    /**
     * Retrieve all expression which have the same action.
     */
    template<class T>
    std::vector<TconstSharedExpression> getExpressionsWithAction()const
    {
        std::vector<TconstSharedExpression> rVec;
        TRuleSet::const_iterator it;
        for( it = m_rules.begin(); it != m_rules.end(); ++it )
        {
            T action = std::dynamic_pointer_cast<typename T::element_type>( (*it)->getAction() );
            if( action )
                rVec.push_back( (*it)->getExpression() );
        }

        return rVec;
    }

    void beginChange();
    void endChange();
signals:
    void changed();

private:
    void dataChanged();

    bool m_onChange = false;

    TRuleSet m_rules;

    TRuleTableMap m_rulesFromSource;

    unsigned int m_nextUid = 0;
};



#endif /* RULETABLE_H_ */
