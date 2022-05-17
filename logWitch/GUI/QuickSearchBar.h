/*
 * QuickSearchBar.h
 *
 *  Created on: 24 Feb 2017
 *      Author: sven
 */

#ifndef LOGWITCH_GUI_QUICKSEARCHBAR_H_
#define LOGWITCH_GUI_QUICKSEARCHBAR_H_

#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include "ActionRules/Action.h"
#include "ActionRules/Expression.h"

#include "LogEntryTableWindow.h"

class LogEntryTableModel;
class QuickSearchLineEdit;

class QuickSearchBar:
    public QWidget
{
  Q_OBJECT
public:
  QuickSearchBar(LogEntryTableWindow* parent, std::shared_ptr<LogEntryTableModel> model, const QString& colorCode = "#81BEF7");
  virtual ~QuickSearchBar();

  enum SearchModes { Regex, Text, Expression};

public slots:
  /**
   * Switches between the different modes for the quicksearch
   * text line.
   */
  void switchSearchMode();

  /**
   * Sets a certain search mode.
   */
  void setSearchMode( SearchModes searchMode );

  /**
   * Updates the view from the quicksearch line
   */
  void updateSearch();

  /**
   * Search the next log entry which will match to out quicksearch expression.
   */
  void searchNext();

  /**
   * Search the previous log entry which will match to out quicksearch expression.
   */
  void searchPrev();

private:

  std::shared_ptr<LogEntryTableModel> m_model;

  LogEntryTableWindow* m_logWindow;

  QuickSearchLineEdit *m_quickSearch;

  SearchModes m_searchMode;

  QPushButton* m_searchModeButton;

  QPushButton* m_markButton;

  TSharedAction m_quickSearchAction;

  TSharedExpression m_quickSearchExp;

  std::string m_myRuleTableName;
};

#endif /* LOGWITCH_GUI_QUICKSEARCHBAR_H_ */
