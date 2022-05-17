/*
 * LogEntryTableWindow.cpp
 *
 *  Created on: May 14, 2011
 *      Author: sven
 */

#include "LogEntryTableWindow.h"

#include <limits>

#include <boost/scoped_ptr.hpp>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

#include "ActionRules/ActionDataRewriter.h"
#include "ActionRules/ActionParser.h"
#include "ActionRules/ExpressionFind.h"
#include "ActionRules/ExpressionMatch.h"
#include "ActionRules/ExpressionParser.h"
#include "ActionRules/ExpressionRegEx.h"
#include "ActionRules/RuleTable.h"
#include "ActionRules/ValueGetterConstQString.h"
#include "ActionRules/ValueGetterLogEntry.h"

#include "GUI/DialogExportOptions.h"
#include "GUI/QuickSearchBar.h"

#include "GUITools/QScrollDownTableView.h"

#include "LogData/EntryToTextFormater.h"
#include "LogData/LogEntryParserModelConfiguration.h"
#include "LogData/LogEntryFactory.h"
#include "LogData/ObjectCache.hxx"

#include "Models/LogEntryTableModel.h"
#include "Models/LogEntryTableModelFileExporter.h"
#include "Models/LogEntryTableFilter.h"
#include "Models/EntryFormatingModel.h"

#include "ContextMenuManipulateHeader.h"
#include "FilterListView.h"

namespace {
  const std::vector<QString> s_colorsForQuickSearch = { "#81BEF7", "#FFC596", "#64FFA9", "#E4AFFF", "#A0FFD4" };
}

void LogEntryTableWindow::updateHeaderSizeToModel(int column, int, int newSize)
{
  if (newSize == 0)
  {
    // This happens if hide is called.
    m_model->setHeaderData(column, Qt::Horizontal, false, 513);
  }
  else
  {
    m_model->setHeaderData(column, Qt::Horizontal, true, 513);
    m_model->setHeaderData(column, Qt::Horizontal, newSize, 512);
  }
}

void LogEntryTableWindow::updateHeaderPositionToModel(int, int, int)
{
  QHeaderView *view = m_tableView->horizontalHeader();
  for (int col = 0; col < view->count(); col++)
    m_model->setHeaderData(view->logicalIndex(col), Qt::Horizontal, col, 514);
}

LogEntryTableWindow::LogEntryTableWindow( boost::shared_ptr<LogEntryTableModel> model, QWidget *parent)
: QMdiSubWindow(parent)
, m_model(model)
, m_splitter( new QSplitter(Qt::Vertical))
, m_myFilterTabs( NULL)
, m_dockFilterShouldDockedTo(NULL)
, m_tableView(new QScrollDownTableView())
, m_nextQsColor(0)
{
  // Parametrize the filtering model, connect it to the table view.
  m_proxyModel = new LogEntryTableFilter(m_tableView);
  m_proxyModel->setSourceModel(m_model.get());

  // Parametrize the table view for the log entries.
  m_timeFormatModel = new EntryFormatingModel(m_tableView);
  m_timeFormatModel->setSourceModel(m_proxyModel);

  m_tableView->setModel(m_timeFormatModel);
  m_tableView->verticalHeader()->setDefaultSectionSize(20);
  m_tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  m_tableView->verticalHeader()->hide();
  m_tableView->setAlternatingRowColors(true);
  m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_tableView->setSelectionMode(QAbstractItemView::ContiguousSelection);

  m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(m_tableView,
      SIGNAL(customContextMenuRequested ( const QPoint &)), this,
      SLOT(contextMenu(const QPoint & )));

  QObject::connect(m_tableView, SIGNAL(doubleClicked ( const QModelIndex &)),
      this, SLOT(onDoubleClick(const QModelIndex & )));

  // Context menu for the HorizontalHeaderView
  m_tableView->horizontalHeader()->setSectionsMovable(true);
  m_tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
  new ContextMenuManipulateHeader(m_tableView->horizontalHeader());

  // Condition the view from the model.
  {
    int count = m_model->columnCount(QModelIndex());
    for (int col = 0; col < count; col++)
    {
      int width = m_timeFormatModel->headerData(col, Qt::Horizontal, 512).value<
          int>();
      bool show = m_timeFormatModel->headerData(col, Qt::Horizontal, 513).value<
          bool>();

      m_tableView->horizontalHeader()->resizeSection(col, width);
      m_tableView->horizontalHeader()->setSectionHidden(col, !show);
    }

    QHeaderView *view = m_tableView->horizontalHeader();

    // This moves the header sections around to fit the needs to the order.
    int visIndex = 0;
    std::set<int> doneColumns;
    for (visIndex = 0; visIndex < view->count(); ++visIndex)
    {
      // First find the column we want to display. This is the column with the lowest number.
      int modelColumn = -1;
      int lowestPrio = std::numeric_limits<int>::max();
      // Find the first element, which has the lowest priority and is not
      // within our doneColumns set. This is then the next column to display.
      for (int i = view->count() - 1; i >= 0; i--)
      {
        if (doneColumns.end() == doneColumns.find(i))
        {
          int prio =
              m_timeFormatModel->headerData(i, Qt::Horizontal, 514).value<int>();
          if (prio <= lowestPrio)
          {
            lowestPrio = prio;
            modelColumn = i;
          }
        }
      }

      doneColumns.insert(modelColumn);
      int visIndexToFind;

      // Find the column in the current header here.
      for (visIndexToFind = 0;
          visIndexToFind < view->count()
              && modelColumn != view->logicalIndex(visIndexToFind);
          ++visIndexToFind)
        ;

      if (visIndexToFind < view->count())
        view->moveSection(visIndexToFind, visIndex);
    }
  }
  // Connect resize signals of the horizontal header to the model update.
  QObject::connect(m_tableView->horizontalHeader(),
      SIGNAL(sectionResized(int , int , int )), this,
      SLOT(updateHeaderSizeToModel(int,int,int)));
  QObject::connect(m_tableView->horizontalHeader(),
      SIGNAL(sectionMoved(int , int , int )), this,
      SLOT(updateHeaderPositionToModel(int,int,int)));

  m_text = new QTextEdit("<b>Log Message viewer</b>", this);
  QObject::connect(m_tableView->selectionModel(),
      SIGNAL(
          selectionChanged ( const QItemSelection & , const QItemSelection & )),
      SLOT(newSelection ( const QItemSelection &, const QItemSelection & )));
  QObject::connect(dynamic_cast<QObject*>(m_model.get()),
      SIGNAL(signalError( QString )), this, SLOT(errorFromModel( QString )));

  // Make the big layout, top is quickearch bar, then log table and at the bottom the log message.
  QVBoxLayout* layout = new QVBoxLayout();
  QWidget *centralWidget = new QWidget(this);

  m_splitter->addWidget(m_tableView);
  m_splitter->addWidget(m_text);
  m_splitter->setStretchFactor(0, 20);

  m_qsLayout = new QVBoxLayout();
  layout->insertLayout(-1, m_qsLayout);
  layout->addWidget(m_splitter);

  addQuicksearchBar();

  centralWidget->setLayout(layout);
  this->setWidget(centralWidget);

  resize(800, 500);

  // Just generate the tab here, because we want to catch all filter entries.
  getTabFilterWidget();
  QMdiSubWindow::setAttribute(Qt::WA_DeleteOnClose, true);
}

namespace
{
  /**
   * This iterator passes only the selected entries to the caller.
   */
  struct DumpSelectedEntries
  {
    DumpSelectedEntries(const std::vector<TconstSharedLogEntry>& entries) :
        entries(entries), it(entries.begin())
    {
    }

    TconstSharedLogEntry operator()()
    {
      if (it != entries.end())
      {
        return *(it++);
      }
      else
        return TconstSharedLogEntry();
    }

    const std::vector<TconstSharedLogEntry> &entries;
    std::vector<TconstSharedLogEntry>::const_iterator it;
  };
}

void LogEntryTableWindow::exportLogfile(const QString &filename)
{
  // Check if we have a selection first.
  const QItemSelection selection = m_tableView->selectionModel()->selection();

  if (!selection.empty())
  {
    const QItemSelectionRange& range = selection.front();
    if (!range.isEmpty())
    {
      boost::scoped_ptr<DialogExportOptions> dlg(new DialogExportOptions);
      dlg->setExportOption(DialogExportOptions::All);

      if (dlg->exec())
      {
        const DialogExportOptions::ExportOption option = dlg->getExportOption();
        if (DialogExportOptions::All == option)
        {
          LogEntryTableModelFileExporter exporter(*m_model);
          exporter.exportTo(filename);
        }
        else if (DialogExportOptions::SelectionWithoutFilter == option
            || DialogExportOptions::Selection == option)
        {
          ExportableIfc::ExportParameters params;
          params.withoutFilter = DialogExportOptions::SelectionWithoutFilter
              == option;
          std::vector<TconstSharedLogEntry> entries;

          std::any lock = m_model->getLock();

          m_timeFormatModel->generateExportList(entries, range.topLeft(),
              range.bottomRight(), params);

          LogEntryTableModelFileExporter exporter(*m_model);
          exporter.exportTo(filename, DumpSelectedEntries(entries));
        }
      }
    }
  }
}


TSharedRuleTable LogEntryTableWindow::getRuleTable()
{
  return m_proxyModel->getRuleTable();
}

void LogEntryTableWindow::addFilter(boost::shared_ptr<LogEntryFilter> flt)
{
  m_proxyModel->addFilter(flt);
}

QModelIndex LogEntryTableWindow::mapToSource(
    const QModelIndex & proxyIndex) const
{
  return m_proxyModel->mapToSource(m_timeFormatModel->mapToSource(proxyIndex));
}

LogEntryTableWindow::~LogEntryTableWindow()
{
  qDebug() << " Window deleted.";
}

void LogEntryTableWindow::errorFromModel(QString error)
{
  QMessageBox msgBox;
  QString errorText;
  errorText += tr("Error received: \n") + error;
  msgBox.setText(errorText);
  msgBox.setInformativeText(
      tr(
          "Close window with the logs now? This will discard all already captured messages. If you want to keep them, press ignore."));
  msgBox.setStandardButtons(QMessageBox::Ignore | QMessageBox::Close);
  msgBox.setDefaultButton(QMessageBox::Ignore);

  int ret = msgBox.exec();
  if (ret == QMessageBox::Close)
  {
    this->deleteLater();
  }
}

void LogEntryTableWindow::setDockForFilter(QDockWidget *dock)
{
  dock->setWidget(getTabFilterWidget());
}

void LogEntryTableWindow::clearTable()
{
  m_model->clearTable();
}

void LogEntryTableWindow::capture(bool active)
{
  m_model->capture(active);
}

QTabWidget *LogEntryTableWindow::getTabFilterWidget()
{
  if (m_myFilterTabs == NULL)
  {
    QTabWidget *tabs = new QTabWidget();

    int attributes =
        m_model->getParserModelConfiguration()->getLogEntryFactory()->getNumberOfFields();
    for (int attr = 0; attr < attributes; attr++)
    {
      // Only show tabs with an active StringCahche.
      if (m_model->getParserModelConfiguration()->getLogEntryFactory()->getCache(
          attr).isCaching())
      {
        FilterListView *view = new FilterListView(this,
            m_model->getParserModelConfiguration(), attr);
        view->addToTabs(tabs, this);
      }
    }
    m_myFilterTabs = tabs;
  }

  return m_myFilterTabs;
}

TSharedCompiledRulesStateSaver LogEntryTableWindow::getCompiledRules()
{
  return TSharedCompiledRulesStateSaver(
      new CompiledRulesStateSaver(m_model->getParserModelConfiguration(),
          getRuleTable()));
}

void LogEntryTableWindow::newSelection(const QItemSelection & selected,
    const QItemSelection &)
{
  if (!selected.empty())
  {
    const QItemSelectionRange& range = selected.front();
    if (!range.isEmpty())
    {
      TconstSharedLogEntry entry = m_model->getEntryByIndex(
          mapToSource(range.topLeft()));
      m_text->setHtml(
          m_model->getParserModelConfiguration()->getEntryToTextFormater()->formatEntry(
              entry));
    }
  }
}

void LogEntryTableWindow::addQuicksearchBar()
{
  QString colorCode = s_colorsForQuickSearch[(m_nextQsColor++) % s_colorsForQuickSearch.size()];
  m_qsLayout->addWidget( new QuickSearchBar( this, m_model, colorCode ) );
}

void LogEntryTableWindow::search(Expression& exp, bool backwards)
{
  if (m_timeFormatModel->rowCount() > 0 )
  {
    int startRow = -1;

    if (m_tableView->selectionModel()->hasSelection())
    {
      QItemSelection selection = m_tableView->selectionModel()->selection();
      // There seems to be a QT Bug, if the ProxyFilterModel is removing the selection
      // hasSelection retruns true while no selection is here!
      if (!selection.isEmpty())
      {
        QModelIndex index = selection.front().topLeft();
        startRow = index.row();
      }
    }

    const int inc = backwards ? -1 : 1;
    int nextRow = startRow + inc;

    if (nextRow >= m_timeFormatModel->rowCount())
      nextRow = 0;
    if (nextRow < 0)
      nextRow = m_timeFormatModel->rowCount() - 1;

    for (; nextRow != startRow;)
    {
      TconstSharedLogEntry entry = m_model->getEntryByIndex(
          mapToSource(m_timeFormatModel->index(nextRow, 0)));
      if (entry && exp.match(entry))
        break;

      // We need to correct startRow, if
      // - we have no selection on entry
      // - the table size may change during search
      if (startRow < 0)
        startRow = nextRow;
      if (startRow >= m_timeFormatModel->rowCount())
        startRow = m_timeFormatModel->rowCount() - 1;

      // Incrementing part
      nextRow += inc;

      if (nextRow >= m_timeFormatModel->rowCount())
        nextRow = 0;
      if (nextRow < 0)
        nextRow = m_timeFormatModel->rowCount() - 1;
    }

    if (nextRow != startRow)
    {
      m_tableView->setCurrentIndex(m_timeFormatModel->index(nextRow, 0));
      m_tableView->setFocus();
    }
  }
}


void LogEntryTableWindow::contextMenu(const QPoint & pt)
{
  // Query the reference timestamp.
  QDateTime dt;
  bool found = false;

  QModelIndex idxClick = mapToSource(m_tableView->indexAt(pt));
  QModelIndex idxClickProxyModel = m_timeFormatModel->mapToSource(
      m_tableView->indexAt(pt));

  for (int i = 0; i < m_model->columnCount(); ++i)
  {
    QModelIndex idx = m_model->index(idxClick.row(), i);
    QVariant var = m_model->data(idx, Qt::DisplayRole);
    qDebug() << " i " << i << " var " << var;
    if (var.type() == QVariant::DateTime)
    {
      dt = var.toDateTime();
      found = true;
      break;
    }
  }

  if (!found)
    return;

  qDebug() << "Using reference time: " << dt;

  QMenu *menu = new QMenu();

  QAction *timeStampsRelative = menu->addAction(tr("Set row as reference"));
  QAction *showTimestamps = menu->addAction(tr("Show absolute times"));
  QAction *showDiffTimes = menu->addAction(tr("Show difference times"));
  QAction *peekFilteredEntries = menu->addAction(tr("Peek filtered"));

  m_model->beginBlockItems();
  QAction *pressed = menu->exec(m_tableView->mapToGlobal(pt));

  if (pressed == timeStampsRelative)
  {
    m_timeFormatModel->setTimeDiffReference(dt);
    m_timeFormatModel->setTimeDiffModeEnabled(true, false);
  }
  else if (pressed == showTimestamps)
  {
    m_timeFormatModel->setTimeDiffModeEnabled(false, false);
  }
  else if (pressed == showDiffTimes)
  {
    m_timeFormatModel->setTimeDiffModeEnabled(true, true);
  }
  else if (pressed == peekFilteredEntries)
  {
    m_proxyModel->showSurroundingLogEntries(idxClickProxyModel, 10);
  }

  m_model->endBlockItems();
  delete menu;
}

void LogEntryTableWindow::onDoubleClick(const QModelIndex & index)
{
  QModelIndex idxClickProxyModel = m_timeFormatModel->mapToSource(index);
  m_proxyModel->showSurroundingLogEntries(idxClickProxyModel, 10);
}
