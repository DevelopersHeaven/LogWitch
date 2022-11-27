/*
 * TimeFormatingModel.cpp
 *
 *  Created on: Dec 30, 2011
 *      Author: sven
 */

#include "EntryFormatingModel.h"

EntryFormatingModel::EntryFormatingModel( QObject *parent )
: QSortFilterProxyModel( parent )
, m_formatString("dd.MM.yyyy hh:mm:ss.zzz")
, m_timeDiffMode( false )
, m_exportOfSourceModel( NULL )
{
}

void EntryFormatingModel::setSourceModel( QAbstractItemModel *model )
{
  // check requirements on model.
  m_exportOfSourceModel = dynamic_cast<ExportableIfc *>(model);
  Q_CHECK_PTR(m_exportOfSourceModel);

  QSortFilterProxyModel::setSourceModel( model );
}

void EntryFormatingModel::setTimeDiffModeEnabled( bool value, bool rowMode )
{
    m_timeDiffMode = value;
    m_timeDiffRowMode = rowMode;
}

void EntryFormatingModel::setTimeDiffReference( QDateTime time )
{
    m_referenceForTimeDiffs = time;
}

void EntryFormatingModel::generateExportList( std::vector<TconstSharedLogEntry>& ls
    , QModelIndex first, QModelIndex last
    , const ExportParameters& param ) const
{
  QModelIndex srcFirst = mapToSource( first );
  QModelIndex srcLast = mapToSource( last );

  m_exportOfSourceModel->generateExportList( ls, srcFirst, srcLast, param );
}

QVariant EntryFormatingModel::data(const QModelIndex &index, int role) const
{
    QVariant var = QAbstractProxyModel::data( index, role );

    // Apply Date / Time formatting here.
    if( role == Qt::DisplayRole )
    {
        if (var.userType() == QVariant::DateTime)
        {
            if( m_timeDiffMode )
            {
                if( m_timeDiffRowMode && ( index.row() <= 0 || index.row() > sourceModel()->rowCount() ) )
                {
                    var = QString("0.000");
                }
                else
                {
                    QDateTime refTime = m_referenceForTimeDiffs;
                    if( m_timeDiffRowMode )
                    {
                        QModelIndex idxBefore = sourceModel()->index( index.row() - 1, index.column() );
                        refTime = sourceModel()->data( idxBefore, role ).toDateTime();
                    }
                    QDateTime yetTime = var.toDateTime();

                    qint64 diffMsec = yetTime.toMSecsSinceEpoch() - refTime.toMSecsSinceEpoch();
                    const bool neg = diffMsec < 0;
                    diffMsec *= neg ? -1 : 1;

                    qint64 secs = diffMsec / 1000 ;
                    qint64 ms = diffMsec % 1000;

                    QString diff = QString("%1.%2").arg( secs ).arg( ms, 3, 10, QChar('0') );
                    if( neg )
                        diff = "-"+diff;
                    var = QVariant( diff );
                }
            }
            else
            {
                var = QVariant( var.toDateTime().toString( m_formatString ) );
            }
        }
    }

    return var;
}
