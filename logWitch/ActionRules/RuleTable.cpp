/*
 * RuleTable.cpp
 *
 *  Created on: Jun 5, 2011
 *      Author: sven
 */

#include "RuleTable.h"

RuleTable::RuleTable()
: m_onChange ( false )
, m_nextUid(0)
{
}

RuleTable::~RuleTable()
{
}

std::string RuleTable::addNewUniqueTable()
{
  do {
    std::string tableNameAsString = std::string("UN+") + std::to_string(++m_nextUid);

    auto it = m_rulesFromSource.find( tableNameAsString );
    if( it != m_rulesFromSource.end() ) {
      continue;
    }
    else {
      it = m_rulesFromSource.insert( TRuleTableMap::value_type(tableNameAsString,TRuleSet()) ).first;
    }
    return it->first;
  } while ( true );
}

void RuleTable::addRule( const std::string& tableName, TSharedRule &rule )
{
    qDebug() << "RuleTable::addRule";

    std::string tableNameAsString( tableName );
    auto it = m_rulesFromSource.find( tableNameAsString );
    if( it == m_rulesFromSource.end() )
    {
        it = m_rulesFromSource.insert( TRuleTableMap::value_type(tableNameAsString,TRuleSet()) ).first;
    }

    it->second.insert( rule );
    m_rules.insert( rule );

    dataChanged();
}

void RuleTable::clear( const std::string& tableName )
{
    std::string tableNameAsString( tableName );
    auto it = m_rulesFromSource.find( tableNameAsString );
    if( it == m_rulesFromSource.end() )
        return;

    m_rulesFromSource.erase( it );

    // rebuild table
    m_rules.clear();
    for (const auto& sourcerule : m_rulesFromSource)
    {
        m_rules.insert( sourcerule.second.begin(), sourcerule.second.end() );
    }

    dataChanged();
}

void RuleTable::beginChange()
{
    m_onChange = true;
}

void RuleTable::endChange()
{
    m_onChange = false;
    emit changed();
}

void RuleTable::dataChanged()
{
    if( !m_onChange )
        emit changed();
}
