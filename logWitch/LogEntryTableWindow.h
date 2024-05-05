/*
 * LogEntryTableWindow.h
 *
 *  Created on: May 14, 2011
 *      Author: sven
 */

#ifndef LOGENTRYTABLEWINDOW_H_
#define LOGENTRYTABLEWINDOW_H_

#include <QDockWidget>
#include <QMdiSubWindow>
#include <QSplitter>
#include <QTextEdit>

#include "ActionRules/RuleTable.h"
#include "ActionRules/CompiledRulesStateSaver.h"

class EntryFormatingModel;
class LogEntryFilter;
class LogEntryTableFilter;
class LogEntryTableModel;
class QScrollDownTableView;
class QSortFilterProxyModel;
class QVBoxLayout;
class QuickSearchBar;

class LogEntryTableWindow: public QMdiSubWindow
{
Q_OBJECT
public:
  LogEntryTableWindow(std::shared_ptr<LogEntryTableModel> model,
      QWidget *parent = nullptr);
  ~LogEntryTableWindow() override;

  QModelIndex mapToSource(const QModelIndex & proxyIndex) const;

  void addFilter(std::shared_ptr<LogEntryFilter> flt);

  TSharedRuleTable getRuleTable();

  QTabWidget *getTabFilterWidget();

  TSharedCompiledRulesStateSaver getCompiledRules();

  void setDockForFilter(QDockWidget *dock);

  void search(Expression& exp, bool backwards);

public slots:
  void newSelection(const QItemSelection & selected,
      const QItemSelection & deselected);

  void clearTable();

  void capture(bool active);

  void errorFromModel(QString error);

  void exportLogfile(const QString &filename);

  /**
   * Opens the context menu at the given position. This will be connected to the table view.
   */
  void contextMenu(const QPoint &);

  void addQuicksearchBar();

private slots:
  /**
   * This function is used to update the size of the header to the model.
   */
  void updateHeaderSizeToModel(int section, int, int newSize);

  void updateHeaderPositionToModel(int section, int, int);

  void onDoubleClick(const QModelIndex & index);

private:
  std::shared_ptr<LogEntryTableModel> m_model;

  QSplitter *m_splitter;

  /// Text field containing the formatted log entry.
  QTextEdit *m_text;

  QTabWidget *m_myFilterTabs;

  QDockWidget *m_dockFilterShouldDockedTo;

  QScrollDownTableView *m_tableView;

  LogEntryTableFilter *m_proxyModel;

  EntryFormatingModel *m_timeFormatModel;

  QVBoxLayout* m_qsLayout;

  unsigned int m_nextQsColor;
};

#endif /* LOGENTRYTABLEWINDOW_H_ */
