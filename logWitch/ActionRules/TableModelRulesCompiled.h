/*
 * TableModelRulesCompiled.h
 *
 *  Created on: Jun 10, 2011
 *      Author: sven
 */

#ifndef TABLEMODELRULESCOMPILED_H_
#define TABLEMODELRULESCOMPILED_H_
#include <vector>

#include <QAbstractTableModel>
#include <QtCore>

#include "ActionRules/FilterRuleCompiled.h"
#include "ActionRules/RuleTable.h"

using std::shared_ptr;

class TableModelRulesCompiled
     :public QAbstractTableModel
{
    Q_OBJECT
public:
    /**
     * Constructs a new Model. The model is bound to a specific parser configuration and
     * a rule table which should be used for filtering.
     * If the configuration or the ruleTable is empty, they will be ignored. The configuration
     * can only be empty if the ruleTable is also empty!
     */
    TableModelRulesCompiled( QObject *parent,
            TSharedConstLogEntryParserModelConfiguration cfg = TSharedConstLogEntryParserModelConfiguration(),
            TSharedRuleTable ruleTable = TSharedRuleTable()  );

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool setData( const QModelIndex &index, const QVariant& value, int role );

    Qt::ItemFlags flags(const QModelIndex & index ) const;

    Qt::DropActions supportedDropActions() const;

    QStringList mimeTypes() const;

    QMimeData *mimeData(const QModelIndexList &indexes) const;

    bool dropMimeData(const QMimeData *data,
            Qt::DropAction action, int row, int column, const QModelIndex &parent);

    /**
     * This appends the given rule. This copies the contents and creates
     * a new one inside the actual context.
     */
    // void appendRule( TSharedFilterRuleCompiled rule );
    void appendRule( const QString &rule );

    /**
     * Retrieves the rule for the given row.
     */
    QString getRule( const int row ) const;

    void removeRule( const QModelIndex &index );

    void removeRules( const QModelIndexList &idxList );

    void removeRules( const std::list<int> &rowList );

    void removeRules( const QMimeData *data );

    static const QString ruleMimeType;

public slots:
    void updateFilterRuleTable();

    void insertEmptyRule();

private:
    /**
     * Checks if the model contains the given rule.
     */
    bool hasRule( const QString &rule ) const;

    /**
     * Retrieve the rows from the mime data. Appends all rows to srcRows, if dataArr
     * has the correct identification. If the scrRows can be extracted,
     * this method returns true.
     */
    bool getRowsFromData( std::list<int> &srcRows, const QByteArray &dataArr ) const;

    QByteArray getIdentification() const;

    void dumpTable() const;

    typedef std::vector<TSharedFilterRuleCompiled> TCompiledRulesTable;
    TCompiledRulesTable m_table;

    static const int m_columnCount = 3;

    /**
     * The configuration this table is for.
     */
    TSharedConstLogEntryParserModelConfiguration m_configuration;

    /**
     * This is the executive element which is the table for filtering.
     */
    TSharedRuleTable m_ruleTable;

};
#endif /* TABLEMODELRULESCOMPILED_H_ */
