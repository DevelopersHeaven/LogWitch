/*
 * LogEntryTableFilter.h
 *
 *  Created on: May 18, 2011
 *      Author: sven
 */

#ifndef LOGENTRYTABLEFILTER_H_
#define LOGENTRYTABLEFILTER_H_

#include <QSortFilterProxyModel>
#include "LogEntryFilterChain.h"
#include "ActionRules/RuleTable.h"

class LogEntryTableModel;

class LogEntryTableFilter
	: public QSortFilterProxyModel
{
	Q_OBJECT
public:
	LogEntryTableFilter( QObject *parent );

	void setSourceModel( QAbstractItemModel *model );

	LogEntryTableModel *getSourceModel() const;

	QVariant data(const QModelIndex &index, int role) const;

	void addFilter( boost::shared_ptr<LogEntryFilter> );

	TSharedRuleTable getRuleTable();

	virtual ~LogEntryTableFilter();

public slots:
	void updateChanges();


protected:
	bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;

private:
	LogEntryTableModel *m_model;

	LogEntryFilterChain m_filterChain;

	TSharedRuleTable m_ruleTable;

	typedef std::vector<TconstSharedExpression> TExpressionVector;

	TExpressionVector m_discardExpressions;
};

#endif /* LOGENTRYTABLEFILTER_H_ */