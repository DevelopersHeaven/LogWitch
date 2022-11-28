#ifndef LOGFILEANALYSER_H
#define LOGFILEANALYSER_H


#include <QtCore>
#include <QMainWindow>

#include "ui_logfileanalyser.h"
#include "GUITools/SignalMultiplexer.h"
#include "ParserActionInterface.h"

class HelpAssistant;

class LogEntryParser_dummy;
class LogEntryTableModel;
class LogEntryTable;
class LogEntryParser;
class QDockWidget;
class QSpinBox;
class WidgetStateSaver;
class QMdiSubWindow;
class FilterRuleSelectionWindow;

namespace logwitch { namespace plugins {
	class LogSourcePlugin;
}}

class LogfileAnalyser : public QMainWindow, public logwitch::ParserActionInterface
{
  Q_OBJECT

public:
  LogfileAnalyser(QWidget *parent = 0);
  ~LogfileAnalyser();

  void newParser(std::shared_ptr<LogEntryParser> parser, bool alreadyInitialized) override;

public slots:
  void subWindowActivated(QMdiSubWindow *);
  void subWindowDestroyed(QObject *obj);
  void showDocumentation();

private:
  void createWindowsFromParser(std::shared_ptr<LogEntryParser> parser, bool alreadyInitialized = false);

  Ui::LogfileAnalyserClass ui;

  QDockWidget *m_myFilterDock;
  QDockWidget *m_myFilterRulesDock;

  FilterRuleSelectionWindow *m_myfilterRuleSelectionWidget;

  WidgetStateSaver *m_stateSaver;
  SignalMultiplexer m_signalMultiplexer;

  std::unique_ptr<HelpAssistant> m_helpAssistant;

  void loadPlugins();
  void loadPlugins(QDir basePath);

  std::list< logwitch::plugins::LogSourcePlugin* > m_logSourcePlugins;

private slots:
  void openLogfile();

  void exportLogfile();

public slots:
  void openLogfile(const QString & fileNames);
};

#endif // LOGFILEANALYSER_H
