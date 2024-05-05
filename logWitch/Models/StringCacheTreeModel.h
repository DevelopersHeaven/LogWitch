/*
 * StringCacheTreeModel.h
 *
 *  Created on: May 15, 2011
 *      Author: sven
 */

#ifndef STRINGCACHETREEMODEL_H_
#define STRINGCACHETREEMODEL_H_
#include "LogData/ObjectCache.hxx"
#include <QAbstractTableModel>
#include "StringCacheTreeItem.h"
#include "Types.h"

class LogEntryFilter;
class LogEntryRemoveFilter;
class QRegularExpression;

class StringCacheTreeModel
	: public QAbstractItemModel
{
	Q_OBJECT
public:
	StringCacheTreeModel( QObject *parent, const GetObjectIF<ObjectCacheQStringSignaller> * cache, int attributeId, const QString &splitString = QString() );

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

//    QVariant headerData(int section, Qt::Orientation orientation,
//                        int role = Qt::DisplayRole) const;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool setData(const QModelIndex &index, const QVariant& value, int role) override;

    std::shared_ptr<LogEntryFilter> getFilter() const;

private:
    void dataChangedToChildren(const QModelIndex &index );

    /**
     * Updates filters starting from the rootNode.
     */
    void updateFilters();

    /**
     * Updates node and all children.
     */
    void updateFilters( StringCacheTreeItem *node );

public slots:
	void newStringElement( TSharedConstQString );

private:
	std::unique_ptr<StringCacheTreeItem> m_rootNode;

	QString m_modelname;

	std::unique_ptr<QRegularExpression> m_splitRegex;

	std::shared_ptr<LogEntryRemoveFilter> m_myFilter;

	TSharedConstQString m_undefinedString;
};

#endif /* STRINGCACHETREEMODEL_H_ */
