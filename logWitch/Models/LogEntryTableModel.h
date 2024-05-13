/*
 * LogEntryTableModel.h
 *
 *  Created on: May 14, 2011
 *      Author: sven
 */

#ifndef LOGENTRYTABLEMODEL_H_
#define LOGENTRYTABLEMODEL_H_

#include <any>
#include <mutex>

#include <QAbstractTableModel>

#include "ExportableIfc.h"

#include "LogData/LogEntry.h"
#include "LogData/LogEntryParserModelConfiguration.h"
#include "LogData/NewLogEntryMessage.h"

class LogEntryParser;
class LogEntryParserModelConfiguration;

/**
 * This is the central model for LogEntries.
 */
class LogEntryTableModel: public QAbstractTableModel, public ExportableIfc
{
  Q_OBJECT

public:
  static const int RawDataRole = Qt::UserRole;

  LogEntryTableModel(std::shared_ptr<LogEntryParser> parser);

  /**
   * Returns a name for this model. The name will be fetched from
   * the parser to identify an opened file or a listening socket.
   * This name should be used to show it to the user.
   */
  QString getModelName() const
  {
    return m_ModelName;
  }

  /**
   * This starts the model. If the startup was finished, using
   * this method will start the parser and everything necessary
   * for populating the model.
   */
  void startModel();

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role) const override;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  bool setHeaderData(int section, Qt::Orientation orientation,
      const QVariant & value, int role) override;

  TconstSharedLogEntry getEntryByRow(const int row) const;

  TconstSharedLogEntry getEntryByIndex(const QModelIndex &index) const;

  TSharedConstLogEntryParserModelConfiguration getParserModelConfiguration() const;

  bool removeRows(int row, int count, const QModelIndex & parent =
      QModelIndex()) override;

  /**
   * This starts block of inserting new items to model. If we are
   * in blocked mode, we will refuse to add new items and we will store the incoming items
   * for later processing. This is useful, if we want to stop scrolling or adding / deleting
   * of items.
   */
  void beginBlockItems();

  void endBlockItems();

  /**
   * Retrieve the locked mutex to ensure we have exclusive access to the model till
   * our action ha been finished. During this time the Model will be used only by
   * the caller and is therefore not being altered by any parser.
   *
   * As long as the returned any contains its value (or is copied to another any)
   * the lock is being held. To release the lock, destroy the content of the any.
   */
  std::any getLock();

  void generateExportList( std::vector<TconstSharedLogEntry>& entries
      , QModelIndex first, QModelIndex last
      , const ExportParameters& param) const override;

public slots:
  /**
   * This slot inserts a new entry to the model. The position for this
   * entry is the end of the model.
   */
  void insertEntry(TconstSharedNewLogEntryMessage);

  /**
   * If this slot is invoked, all stored log entries will be dropped.
   */
  void clearTable();

  /**
   * This sets the maximum numbers of entries to the model. If the model
   * receives more entries it will drop older entries first.
   *
   * @param count Numbers of entries to keep at maximum. If set to 0, disables
   *              this feature.
   */
  void setMaxEntries(int count);

  /**
   * Enables or disables capturing of new log entries.
   * @param active If true, the model will store incoming log entries.
   */
  void capture(bool active);

  /**
   * This slot receives errors from the parser and handles it over to
   * the view or something elese.
   */
  void signalErrorFromParser(QString error);

signals:
  /**
   * This signal is emitted if an error from a parser was received.
   * This signal should be used to inform the user or do something else.
   *
   * @param error A brief description of the occurred error.
   */
  void signalError(QString error);

private:
  /**
   * Implementation of removeReows.
   *
   * This method does not do any locking, this has to be ensured by
   * the caller.
   */
  bool removeRows_unlocked(int row, int count, const QModelIndex & parent =
      QModelIndex());

  typedef std::vector<TconstSharedLogEntry> TLogEntryTable;

  TLogEntryTable m_table;

  TSharedLogEntryParserModelConfiguration m_modelConfiguration;

  std::shared_ptr<LogEntryParser> m_entryLoader;

  QString m_ModelName;

  mutable std::recursive_mutex m_mutex;

  bool m_captureActive = true;

  int m_maxNumberOfEntries = 0;

  bool m_blockInsertingMessages = false;

  std::list<TconstSharedNewLogEntryMessage> m_blockedInsertedMessages;
};

#endif /* LOGENTRYTABLEMODEL_H_ */
