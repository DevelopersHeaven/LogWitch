/*
 * TableModelRulesCompiledCompiled.cpp
 *
 *  Created on: Jun 10, 2011
 *      Author: sven
 */

#include "TableModelRulesCompiled.h"

#include <QtGui>

#include "ActionRules/ActionDataRewriter.h"
#include "ActionRules/FilterRuleCompiled.h"
#include "FilterRuleCompiled.h"

using namespace std::placeholders;

const QString TableModelRulesCompiled::ruleMimeType("application/x-de.steckmann.LogWitch.rule");

TableModelRulesCompiled::TableModelRulesCompiled( QObject *parent, TSharedConstLogEntryParserModelConfiguration cfg, TSharedRuleTable ruleTable )
    :QAbstractTableModel( parent )
    , m_configuration( cfg )
    , m_ruleTable( ruleTable )
{
}

TableModelRulesCompiled::~TableModelRulesCompiled()
{
}

int TableModelRulesCompiled::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_table.size() + 1;
}

int TableModelRulesCompiled::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_columnCount;
}

QVariant TableModelRulesCompiled::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
     return QVariant();

    if (index.column() >= m_columnCount
         || index.column() < 0
         || index.row() < 0
         || index.row() > int(m_table.size() ) )
     return QVariant();

    if( index.row() == int(m_table.size()) )
    {
        if( role == Qt::DisplayRole && index.column() == 0 )
            return QString(" * ");

        return QVariant();
    }

    TSharedFilterRuleCompiled row = m_table[index.row()];

    if( role == Qt::ToolTipRole )
    {
        QString tooltip;
        if( !row->isExpressionOk() )
        {
            tooltip = tr("Expression has errors: ") + row->getExpressionError();
        }

        if( !row->isActionOk() )
        {
            if( !tooltip.isEmpty() )
                tooltip.append("\n");

            tooltip += tr("Action has errors: ") + row->getActionError();
        }

        if( !row->validWithinContext() && m_configuration )
        {
            if( !tooltip.isEmpty() )
                tooltip.append("\n");
            tooltip += tr("Not valid with this parser: At least one field not found!");
        }

        return tooltip;
    }

    if( role == Qt::FontRole )
    {
        if( !row->validWithinContext() && m_configuration )
        {
            QFont font;
            font.setStrikeOut(true);
            return font;
        }
    }

    if( index.column() == 0 )
    {
        if (    role == Qt::DisplayRole
            ||  role == Qt::EditRole )
        {
            return row->expressionString();
        }

        if( !row->isExpressionOk())
        {
            if( role == Qt::BackgroundColorRole )
            {
                return QColor( Qt::red );
            }
        }
    }
    else if( index.column() == 1 )
    {
        if( role == Qt::EditRole )
        {
            return row->actionString();
        }

        if( !row->isActionOk())
        {
            if( role == Qt::BackgroundColorRole )
            {
                return QColor( Qt::red );
            }
        }

        if( row->getActionDisplayer() )
        {
            return row->getActionDisplayer()->toDisplay( role );
        }
        else if (role == Qt::DisplayRole )
        {
            return row->actionString();
        }
    }
    else if( index.column() == 2 )
    {
        if( role == Qt::EditRole || role == Qt::DisplayRole )
        {
            return row->getUserDescription();
        }
    }

    return QVariant();
}

Qt::ItemFlags TableModelRulesCompiled::flags(const QModelIndex &index  ) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);

     if (index.isValid())
     {
         if( index.row() < rowCount() )
             flags |= Qt::ItemIsDragEnabled;

         return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable
              | Qt::ItemIsDropEnabled | flags;
     }
     else
         return Qt::ItemIsDropEnabled | flags;
}

QVariant TableModelRulesCompiled::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if( section == 0 )
                return QString(tr("Expression"));
            if( section == 1 )
                return QString(tr("Action"));
            if( section == 2 )
                return QString(tr("Description"));
        }
    }

    return QVariant();
}


bool TableModelRulesCompiled::setData( const QModelIndex &index, const QVariant& value , int role  )
{
    if( !index.isValid())
        return false;

    if (role != Qt::EditRole)
        return false;

    if (index.column() >= m_columnCount
         || index.column() < 0
         || index.row() < 0
         || index.row() > int(m_table.size() ) )
        return false;

    if( index.row() == int(m_table.size()) )
    {
        if( value.toString().length() == 0 )
            return false;

        TSharedFilterRuleCompiled rule = TSharedFilterRuleCompiled( new FilterRuleCompiled( "", m_configuration ) );

        beginInsertRows( QModelIndex(), m_table.size(), m_table.size() );
        m_table.push_back( rule );
        endInsertRows();
    }

    TSharedFilterRuleCompiled row = m_table[index.row()];

    if( index.column() == 0 )
    {
        row->expressionString( value.toString() );
        updateFilterRuleTable();
        return true;
    }
    else if( index.column() == 1 )
    {
        row->actionString( value.toString() );
        updateFilterRuleTable();
        return true;
    }
    else if( index.column() == 2 )
    {
        row->setUserDescription( value.toString() );
        return true;
    }

    return false;
}

void TableModelRulesCompiled::updateFilterRuleTable()
{
    if( m_ruleTable )
    {
        m_ruleTable->beginChange();
        m_ruleTable->clear("filters");

        TCompiledRulesTable::iterator it;
        for( it = m_table.begin(); it != m_table.end(); ++it)
        {
            TSharedRule rule = (*it)->getCompiledRule();
            if( rule && rule->isValid() )
                m_ruleTable->addRule( "filters", rule );
        }
        m_ruleTable->endChange();
    }
}

namespace
{
    bool ruleEquals( const TSharedFilterRuleCompiled &rule, const QString &ruleStr )
    {
        return rule->toString() == ruleStr;
    }
}

QString TableModelRulesCompiled::getRule( const int row ) const
{
    if( row >= 0 && row < int(m_table.size()) )
        return m_table[row]->toString();
    else
        return QString();
}

bool TableModelRulesCompiled::hasRule( const QString &rule ) const
{
    TCompiledRulesTable::const_iterator it;
    it = std::find_if(m_table.begin(), m_table.end(), std::bind( &ruleEquals, _1, rule ) );
    return it != m_table.end();
}

void TableModelRulesCompiled::appendRule( const QString &rule )
{
    if( hasRule( rule ) )
        return;

    TSharedFilterRuleCompiled cr( new FilterRuleCompiled( rule, m_configuration ) );

    int newPos = m_table.size();
    beginInsertRows( QModelIndex(), newPos, newPos );
    m_table.push_back( cr );
    endInsertRows();

    updateFilterRuleTable();
}

void TableModelRulesCompiled::removeRule( const QModelIndex &index )
{
    if (!index.isValid())
     return;

    if ( index.row() < 0
         || index.row() >= int(m_table.size() ) )
     return;

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    m_table.erase( m_table.begin() + index.row() );
    endRemoveRows();

    updateFilterRuleTable();
}

void TableModelRulesCompiled::removeRules( const std::list<int> &rowList )
{
    // Removing many is complex, because the rows within idxList always reference
    // to the state before removing a row. So we sort everything in reverse order
    // and then start removing from the back on.
    std::list<int> sortRowList( rowList );
    sortRowList.sort();

    int toRemoveLast = -1;

    for( std::list<int>::const_reverse_iterator it = sortRowList.rbegin()
        ; it != sortRowList.rend(); ++it )
    {
        if(   toRemoveLast != *it
           && *it >= 0
           && *it <int( m_table.size()) )
        {
            beginRemoveRows(QModelIndex(), *it, *it);
            m_table.erase( m_table.begin() + *it );
            endRemoveRows();
            toRemoveLast = *it;
        }
    }

    updateFilterRuleTable();
}

void TableModelRulesCompiled::removeRules( const QModelIndexList &idxList )
{
    // Removing many is complex, because the rows within idxList always reference
    // to the state before removing a row. So we sort everything in reverse order
    // and then start removing from the back on.
    std::list<int> rowList;
    QModelIndexList::const_iterator it;
    for( it = idxList.begin(); it != idxList.end(); ++it )
        rowList.push_back( it->row() );

    removeRules( rowList );
}

void TableModelRulesCompiled::insertEmptyRule()
{
    TSharedFilterRuleCompiled row = TSharedFilterRuleCompiled( new FilterRuleCompiled( "", m_configuration) );

    int newPos = m_table.size();
    beginInsertRows( QModelIndex(), newPos, newPos );
    m_table.push_back( row );
    endInsertRows();
}

Qt::DropActions TableModelRulesCompiled::supportedDropActions() const
{
     return Qt::CopyAction;
}

QStringList TableModelRulesCompiled::mimeTypes() const
{
    QStringList types;
    types << ruleMimeType;
    types << QLatin1String("text/plain");
    return types;
}

void TableModelRulesCompiled::dumpTable() const
{
    qDebug()<<"Dumping rule table:";
    TCompiledRulesTable::const_iterator it;
    for( it = m_table.begin(); it != m_table.end(); ++it )
    {
        qDebug() << (*it)->toString();
    }
}

bool TableModelRulesCompiled::getRowsFromData( std::list<int> &srcRows, const QByteArray &dataArr ) const
{
    const QByteArray id = getIdentification();
    QByteArray data( dataArr );
    QDataStream stream( &data, QIODevice::ReadOnly );

    // We will move data here, because this data is coming from us!
    if( dataArr.startsWith(id ) )
    {
        stream.skipRawData( id.length() );
        while( !stream.atEnd() )
        {
            int row;
            stream >> row;
            srcRows.push_back( row );
        }

        return true;
    }

    return false;
}

bool TableModelRulesCompiled::dropMimeData(const QMimeData *data,
    Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    int insertPos = (parent.row() < 0) ? m_table.size() : parent.row();
    if( insertPos >= int(m_table.size()) )
        insertPos = m_table.size();

    if( data->hasFormat( ruleMimeType ) )
    {
        // We will move data here, because this data is coming from us!
        std::list<int> srcRows;
        if( getRowsFromData(srcRows,data->data( ruleMimeType ) ) )
        {
            srcRows.sort();

            // Now extract the rows and place them to the new position.
            std::list<TSharedFilterRuleCompiled> rules;
            for( std::list<int>::reverse_iterator it = srcRows.rbegin(); it != srcRows.rend(); ++it )
            {
                int rowMapped = *it;
                if( rowMapped < int(m_table.size()) && rowMapped >= 0)
                {
                    rules.push_front( m_table[rowMapped] );
                    m_table.erase( m_table.begin() + rowMapped );
                    if( rowMapped <= insertPos )
                    {
                        if( insertPos > 0 )
                            insertPos--;
                    }
                }
            }

            for( std::list<TSharedFilterRuleCompiled>::reverse_iterator it = rules.rbegin()
                ; it != rules.rend()
                ; ++it )
            {
                m_table.insert( m_table.begin() + insertPos, *it );
            }

            updateFilterRuleTable();
            return true;
        }
    }

    if( data->hasText() )
    {
        QString ruleText = data->text();
        QStringList singleRuleTexts = ruleText.split( '\n' );

        for( QStringList::iterator it = singleRuleTexts.begin()
            ; it != singleRuleTexts.end()
            ; ++it )
        {
            if( !hasRule( *it ) )
            {
                TSharedFilterRuleCompiled rule = TSharedFilterRuleCompiled( new FilterRuleCompiled( *it, m_configuration ) );

                if( rule->isActionOk() && rule->isExpressionOk() )
                {
                    beginInsertRows( QModelIndex(), insertPos, insertPos );
                    m_table.insert( m_table.begin() + insertPos, rule );
                    endInsertRows();
                    insertPos++;
                }
            }
        }

        updateFilterRuleTable();
        return true;
    }

    return false;
}

void TableModelRulesCompiled::removeRules( const QMimeData *data )
{
    if( data->hasFormat( ruleMimeType ) )
    {
        // We will move data here, because this data is coming from us!
        std::list<int> srcRows;
        if( getRowsFromData(srcRows,data->data( ruleMimeType ) ) )
            removeRules( srcRows );
    }
}

QByteArray TableModelRulesCompiled::getIdentification() const
{
    QByteArray encSource;
    QDataStream srcStream(&encSource, QIODevice::WriteOnly);
    srcStream << QCoreApplication::applicationPid() << ((qint64)this);
    return encSource;
}

QMimeData *TableModelRulesCompiled::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.count() <= 0)
        return 0;
    QStringList types = mimeTypes();
    if (types.isEmpty())
        return 0;
    QMimeData *data = new QMimeData();
    QString format = types.at(0);

    QString encoded;
    QTextStream stream(&encoded, QIODevice::WriteOnly);

    QByteArray encSource;
    QDataStream srcStream(&encSource, QIODevice::WriteOnly);

    std::list<int> rowSet;

    QModelIndexList::ConstIterator it = indexes.begin();
    bool lEnd = false;
    for (; it != indexes.end(); ++it)
    {
        const int row = (*it).row();
        if( find( rowSet.begin(), rowSet.end(), row ) == rowSet.end() )
        {
            rowSet.push_back( row );
            if( row >= 0 && row < int(m_table.size()) )
            {
                if( lEnd )
                    stream << '\n';
                stream << m_table[row]->toString();
                lEnd = true;

                srcStream << row;
            }
        }
    }

    data->setData( format, encSource.prepend( getIdentification() ) );
    data->setText( encoded );
    return data;
}
