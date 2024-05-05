/*
 * TimeFormatingModel.h
 *
 *  Created on: Dec 30, 2011
 *      Author: sven
 */

#ifndef ENTRYFORMATINGMODEL_H_
#define ENTRYFORMATINGMODEL_H_
#include <QDateTime>
#include <QSortFilterProxyModel>
#include "ExportableIfc.h"

class EntryFormatingModel
: public QSortFilterProxyModel // This should be moved to the identity model in QT4.8 ....
, public ExportableIfc
{
    Q_OBJECT
public:
    EntryFormatingModel( QObject *parent );

    QVariant data(const QModelIndex &index, int role) const override;

    void setSourceModel(QAbstractItemModel *model) override;

    /**
     * Enables or disables the TimeDiff mode.
     */
    void setTimeDiffModeEnabled( bool value, bool rowMode );

    /**
     * Sets the reference row for the differences.
     */
    void setTimeDiffReference( QDateTime time );


    void generateExportList( std::vector<TconstSharedLogEntry>& entries
        , QModelIndex first, QModelIndex last
        , const ExportParameters& param) const override;
private:
    QString m_formatString;

    /**
     * This is the row which should be used for the TimeDiff mode. If it is set
     * to a value < 0, the timeDiff mode will work only between two entries.
     */
    QDateTime m_referenceForTimeDiffs;

    /**
     * Enable the timeDiffMode.
     */
    bool m_timeDiffMode;

    bool m_timeDiffRowMode;

    ExportableIfc *m_exportOfSourceModel;
};

#endif /* TIMEFORMATINGMODEL_H_ */
