/*
 * StringCacheTreeModel.cpp
 *
 *  Created on: May 15, 2011
 *      Author: sven
 */

#include "StringCacheTreeModel.h"

#include <QtCore/QtCore>
#include "LogEntryRemoveFilter.h"
#include <QtGui>

StringCacheTreeModel::StringCacheTreeModel( QObject *parent, const GetObjectIF<ObjectCacheQStringSignaller> * cache, int attributeId, const QString &splitString  )
	: QAbstractItemModel( parent )
	, m_splitRegex( )
	, m_myFilter( new LogEntryRemoveFilter(attributeId) )
	, m_undefinedString( new QString("Undefined") )
{
	TSharedConstQString rootNode( new QString("RootNode"));
	m_rootNode.reset( new StringCacheTreeItem( rootNode, rootNode ) );

	if( !splitString.isEmpty() )
		m_splitRegex.reset(new QRegularExpression(splitString));

    QObject::connect(cache, SIGNAL(newElement( TSharedConstQString )),
                     this, SLOT(newStringElement( TSharedConstQString ) ) );
}

StringCacheTreeModel::~StringCacheTreeModel()
{

}

std::shared_ptr<LogEntryFilter> StringCacheTreeModel::getFilter() const
{
	return m_myFilter;
}

QVariant StringCacheTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    StringCacheTreeItem *item = static_cast<StringCacheTreeItem*>(index.internalPointer());

    if( role == Qt::ForegroundRole )
    {
        StringCacheTreeItem::CheckState state = item->getCheckState();

        if( state.checked )
            return QBrush( Qt::darkGreen );
        else
            return QBrush( Qt::darkGray );
    }
    else if( role == Qt::CheckStateRole )
	{
        StringCacheTreeItem::CheckState state = item->getCheckState();
		if( state.forced )
			return QVariant();

		return item->getChecked();
	}
	else if (role == Qt::DisplayRole)
		return *(item->getString());


    return QVariant();
}

bool StringCacheTreeModel::setData( const QModelIndex &index, const QVariant& value, int role )
{
	if( !index.isValid())
		return false;

	if( role == Qt::CheckStateRole || role == 512 )
	{
        StringCacheTreeItem *item = static_cast<StringCacheTreeItem*>( index.internalPointer() );

        if( role == 512 )
        {

            int val = value.toInt( );
            qDebug() << "val = " << val;
            if( val == 1)
                item->setCheckedSelf( StringCacheTreeItem::Checked );
            else if( val == 0 )
                item->setCheckedSelf( StringCacheTreeItem::Unchecked );
            else if( val == 2 )
                item->checkOnlyThisElement( true );
            else if( val == 3 )
                item->checkOnlyThisElement( false );
            else if( val == 4 )
                item->getRootElement()->recursiveSetTree( StringCacheTreeItem::Inherit );
            else if( val == 5 )
                item->recursiveSetTree( StringCacheTreeItem::Inherit, StringCacheTreeItem::Inherit );
            else if( val == 6 )
                item->recursiveSetTree( StringCacheTreeItem::Unchecked, StringCacheTreeItem::Inherit );

        }
        else
            item->nextChecked();

        QModelIndex node = index;
        while( node.isValid() )
        {
            emit dataChanged( node, node );
            node = node.parent();
        }

        dataChangedToChildren(index);

        updateFilters();
        return true;
	}

	return false;
}

void StringCacheTreeModel::updateFilters()
{
    m_myFilter->startChange();
    updateFilters( m_rootNode.get() );
    m_myFilter->endChange();
}

void StringCacheTreeModel::updateFilters( StringCacheTreeItem *node )
{
    // Step 1: Update current node
    if( node->getOriginalString() != m_undefinedString )
    {
        if( node->getCheckState().checked )
            m_myFilter->removeEntry( node->getOriginalString() );
        else
            m_myFilter->addEntry( node->getOriginalString() );
    }

    // Step 2: Traverse all child nodes
    for( int i = 0; i < node->childCount(); ++i )
        updateFilters( node->child( i ) );
}


void StringCacheTreeModel::dataChangedToChildren(const QModelIndex &index)
{
	emit dataChanged(index, index);

	if( !hasChildren(index) )
		return;

	for(int childRow = 0; childRow < rowCount(index); ++childRow)
	{
	    QModelIndex childIndex = this->index(childRow, 0, index);
	    dataChangedToChildren( childIndex );
	}
}

Qt::ItemFlags StringCacheTreeModel::flags(const QModelIndex &/* index*/ ) const
{

	return /*QAbstractItemModel::flags( index ) | */ Qt::ItemIsAutoTristate | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;

}

//QVariant StringCacheTreeModel::headerData(int section, Qt::Orientation orientation,
//                    int role = Qt::DisplayRole) const
//{
//
//}

QModelIndex StringCacheTreeModel::index(int row, int column,
                  const QModelIndex &parent /*= QModelIndex()*/) const
{
	 if (!hasIndex(row, column, parent))
		 return QModelIndex();

	 StringCacheTreeItem *parentItem;

	 if (!parent.isValid())
		 parentItem = m_rootNode.get();
	 else
		 parentItem = static_cast<StringCacheTreeItem*>(parent.internalPointer());

	 StringCacheTreeItem *childItem = parentItem->child(row);
	 if (childItem)
		 return createIndex(row, column, childItem);
	 else
		 return QModelIndex();
}

QModelIndex StringCacheTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    StringCacheTreeItem *childItem = static_cast<StringCacheTreeItem*>(index.internalPointer());
    StringCacheTreeItem *parentItem = childItem->parent();

    if (parentItem == m_rootNode.get())
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int StringCacheTreeModel::rowCount(const QModelIndex &parent /* = QModelIndex() */) const
{
    StringCacheTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootNode.get();
    else
        parentItem = static_cast<StringCacheTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int StringCacheTreeModel::columnCount(const QModelIndex &/* parent = QModelIndex()*/ ) const
{
	return 1;
}

void StringCacheTreeModel::newStringElement( TSharedConstQString string )
{
	//qDebug() << "Received new string: " << *string ;
	if( m_splitRegex )
	{
		QStringList list = string->split( *m_splitRegex );
		StringCacheTreeItem *node = m_rootNode.get();
		QModelIndex idx;

		while( !list.empty() )
		{
			QString str = list.front();
			list.pop_front();

			int i = 0;
			const int count = node->childCount();
			for( ; i < count; i++ )
			{
				if( *(node->child(i)->getString()) == str )
				{
					node = node->child(i);
					idx = index(i,0,idx);
					break;
				}
			}

			if( i == count )
			{
				beginInsertRows( idx, node->childCount(), node->childCount() );
				StringCacheTreeItem *newNode = new StringCacheTreeItem(
						  list.empty() ? string : m_undefinedString
						, TSharedConstQString( new QString(str) )
						, node );
				node->appendChild( newNode );
				node = newNode;
				idx = index(i,0,idx);
				endInsertRows();
			}
			if( list.empty() && i < count)
			{
				// Here we should replace the Undefined string with the real one. This case
				// happens if a deeper hierarchy was inserted before.
				node->setOriginalString( string );
			}
		}

		// Now this is a special case, for filtering we also need for in between elements the right element.
		// eg: S1.S2.S3 is coming first and the S1, so S1 node has the string S1.S2.S3 as original string.
		// this must be omitted!! So check this. We may also have problems with the sourcestring therefore.
		// We should generate a new one in this case.
	}
	else
	{
		beginInsertRows( QModelIndex(), m_rootNode->childCount(), m_rootNode->childCount() );
		m_rootNode->appendChild( new StringCacheTreeItem( string, string, m_rootNode.get() ) );
		endInsertRows();
	}

	updateFilters();
}

