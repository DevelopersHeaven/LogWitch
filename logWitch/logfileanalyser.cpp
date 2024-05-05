#include "logfileanalyser.h"

#include <QtGui>
#include <QFileDialog>
#include <QHostAddress>
#include <QLabel>
#include <QMessageBox>
#include <QSpinBox>

#include "LogData/ParserStreamGetterFile.h"

#include "Models/LogEntryTableModel.h"
#include "LogEntryTableWindow.h"

#include "LogData/LogEntryParser_Logfile.h"
#include "LogData/LogEntryParser_LogfileLWI.h"
#include "GUITools/WidgetStateSaver.h"
#include "GUITools/GetSetStateSaver.hxx"
#include "GUITools/SignalMultiplexerStateApplier.h"
#include "ActionRules/FilterRuleSelectionWindow.h"
#include "Help/HelpAssistant.h"

#include "Plugins/LogSource/Interface/LogSourcePlugin.h"

LogfileAnalyser::LogfileAnalyser(QWidget *parent)
  : QMainWindow(parent),
    m_myFilterDock(nullptr),
    m_stateSaver(nullptr),
    m_helpAssistant(new HelpAssistant)
{
  ui.setupUi(this);

  loadPlugins();

  m_stateSaver = new WidgetStateSaver(this);
  m_stateSaver->addElementToWatch(&m_signalMultiplexer,
                                  SignalMultiplexerStateApplier::generate(&m_signalMultiplexer));

  QObject::connect(ui.actionHelp, &QAction::triggered, this, &LogfileAnalyser::showDocumentation);
  QObject::connect(ui.actionOpen, &QAction::triggered, [this] { openLogfile(); });
  QObject::connect(ui.actionExportLogfile, &QAction::triggered, this, &LogfileAnalyser::exportLogfile);
  QObject::connect(ui.mdiArea, &QMdiArea::subWindowActivated, this,
                   &LogfileAnalyser::subWindowActivated);

  m_myFilterRulesDock = new QDockWidget(tr("Filter Rules"), this);
  m_myFilterRulesDock->setAllowedAreas(Qt::AllDockWidgetAreas);
  m_myfilterRuleSelectionWidget = new FilterRuleSelectionWindow(m_myFilterRulesDock);
  m_myFilterRulesDock->setWidget(m_myfilterRuleSelectionWidget);
  addDockWidget(Qt::RightDockWidgetArea, m_myFilterRulesDock);

  m_stateSaver->addElementToWatch(m_myfilterRuleSelectionWidget,
                                  GetSetStateSaver<FilterRuleSelWndStateSaverTypes>::generate());
  m_stateSaver->addElementToWatch(ui.actionCapture,
                                  GetSetStateSaver<QQctionCheckedSaverTypes>::generate());
}

LogfileAnalyser::~LogfileAnalyser()
{
	for ( auto plugin: m_logSourcePlugins )
		delete plugin;
}

void LogfileAnalyser::loadPlugins()
{
	// Qt Network functionality is used by some plugins (which are deployed to
	// `lib/plugins`), but the CMake function `qt_generate_deploy_app_script()`
	// only works for targets that are deployed to `bin`.
	// As a consequence, the main binary must link against all necessary Qt
	// libraries so `windeployqt` can detect and include them.
	// To ensure this, instantiate one Qt Network class here.
	QHostAddress dummy;

	// In case of building release
	QDir pluginsDir(qApp->applicationDirPath());
	if (pluginsDir.cd(LW_PLUGIN_DIR))
		loadPlugins(pluginsDir);
	else
	{
		qDebug() << " Unable to find relative plugin directory: " << LW_PLUGIN_DIR;
		qDebug() << " Falling back to developer directory search in application binary path";

		// In case of building locally and developing
		QDir pluginsDir(qApp->applicationDirPath());
		pluginsDir.cd("Plugins");
		loadPlugins(pluginsDir);
	}
}

void LogfileAnalyser::loadPlugins(QDir basePath)
{
	qDebug() << "loadPlugins from " << basePath.absolutePath();

	for (QString fileName: basePath.entryList(QDir::Files))
	{
		QString pluginFileName = basePath.absoluteFilePath (fileName);
		QPluginLoader pluginLoader (pluginFileName);
		QObject *plugin = pluginLoader.instance ();
		if (plugin)
		{
			qDebug() << "Found plugin: " << fileName;
			auto logsourceplugin = qobject_cast<logwitch::plugins::LogSourcePlugin *> (plugin);
			if (logsourceplugin)
			{
				qDebug() << " Plugin is of type LogSourcePlugin";
				logsourceplugin->attachParserAction(this);
				QToolBar* toolbar = logsourceplugin->getToolbar();
				if (toolbar)
				  addToolBar(toolbar);

			  logsourceplugin->fillMenu( ui.menuLogSources );

				m_logSourcePlugins.push_back(logsourceplugin);
			}
		}
		else
		{
			//qDebug() << pluginFileName << " Error: " << pluginLoader.errorString();
		}
	}

	for (const QString& dirName: basePath.entryList(QDir::Dirs|QDir::NoDotAndDotDot))
	{
		QDir nextDir( basePath );
		nextDir.cd(dirName);
		loadPlugins( nextDir );
	}
}

void LogfileAnalyser::subWindowDestroyed(QObject *obj)
{
  qDebug() << "subWindowDestroyed";
  // Window closed, remove docks
  if (ui.mdiArea->subWindowList().size() == 0)
    m_stateSaver->switchState(nullptr);
  else
  {
    QMdiSubWindow *wnd = ui.mdiArea->activeSubWindow();
    if (wnd)
      m_stateSaver->switchState(wnd);
    else
      m_stateSaver->switchState(ui.mdiArea->subWindowList().front());
  }

  m_stateSaver->deregisterFocusObject(obj, false);
}

void LogfileAnalyser::subWindowActivated(QMdiSubWindow *obj)
{
  qDebug() << "subWindowActivated";
  if (obj != nullptr)
    m_stateSaver->switchState(obj);
}

void LogfileAnalyser::showDocumentation()
{
  qDebug() << "ShuwDocumentation";

  m_helpAssistant->showDocumentation("index.html");
}

void LogfileAnalyser::openLogfile(const QString &fileName)
{
  std::shared_ptr<LogEntryParser> parser(
      new LogEntryParser_LogfileLWI(fileName));
  if (!parser->initParser())
  {
    qDebug() << " LWI-Parser failed: " << parser->getInitError();
    parser.reset(
        new LogEntryParser_Logfile(
            std::make_shared<ParserStreamGetterFile>(fileName)));
    if (parser->initParser())
      createWindowsFromParser(parser, true);
    else
    {
      QMessageBox msgBox;
      QString errorText;
      errorText += tr("Error while initializing parser: \n")
          + parser->getInitError();
      msgBox.setText(errorText);
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.exec();
    }
  }
  else
  {
    createWindowsFromParser(parser, true);
  }
}

void LogfileAnalyser::openLogfile()
{
  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (dialog.exec())
  {
    QStringList fileNames = dialog.selectedFiles();
    openLogfile(fileNames.first());
  }
}

void LogfileAnalyser::newParser(std::shared_ptr<LogEntryParser> parser, bool alreadyInitialized)
{
	createWindowsFromParser( parser, alreadyInitialized);
}

void LogfileAnalyser::createWindowsFromParser(std::shared_ptr<LogEntryParser> parser, bool alreadyInitialized)
{
  if (!alreadyInitialized && !parser->initParser())
  {
    // Parser has an error while init, so view message box to inform user and do
    // not create a new window.
    QMessageBox msgBox;
    QString errorText;
    errorText += tr("Error while initializing parser: \n") + parser->getInitError();
    msgBox.setText(errorText);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
    return;
  }

  std::shared_ptr<LogEntryTableModel> model(new LogEntryTableModel(parser));

  LogEntryTableWindow *wnd = new LogEntryTableWindow(model, ui.mdiArea);
  m_signalMultiplexer.setObject(wnd);
  m_signalMultiplexer.connect(ui.actionClearLogTable, SIGNAL(triggered()), wnd, SLOT(clearTable()));
  m_signalMultiplexer.connect(ui.actionCapture, SIGNAL(toggled(bool)), wnd, SLOT(capture(bool)));
  m_signalMultiplexer.connect(ui.actionAddQuicksearchFilter, SIGNAL(triggered()), wnd, SLOT(addQuicksearchBar()));

  wnd->setWindowState(Qt::WindowMaximized);
  wnd->setAttribute(Qt::WA_DeleteOnClose);
  wnd->setWindowTitle(parser->getName());
  wnd->show();

  ui.actionCapture->setChecked(true);

  /*
   * We want to open the Dock the first time we create a window.
   * The advantage of doing so is the correct size for the inner
   * filter tab widget which is set to the dock widget.
   */
  if (!m_myFilterDock)
  {
    m_myFilterDock = new QDockWidget(tr("FilterSettings"), this);
    m_myFilterDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_stateSaver->addElementToWatch(m_myFilterDock, DockWidgetStateSaver::generate());

    wnd->setDockForFilter(m_myFilterDock);
    addDockWidget(Qt::RightDockWidgetArea, m_myFilterDock);
  }
  else
  {
    wnd->setDockForFilter(m_myFilterDock);
  }

  QObject::connect(wnd, &QObject::destroyed, this,
                   &LogfileAnalyser::subWindowDestroyed);

  model->startModel();

  TSharedCompiledRulesStateSaver state = wnd->getCompiledRules();
  m_myfilterRuleSelectionWidget->setWindow(state);

}


void LogfileAnalyser::exportLogfile()
{
  LogEntryTableWindow *wnd = nullptr;
  wnd = dynamic_cast<LogEntryTableWindow *>(ui.mdiArea->activeSubWindow());

  if (wnd)
  {
    QString fname;
    fname += "exportedLogfile " + QDateTime::currentDateTime().toString("yyyyMMdd hhmm") + ".log";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Logfile"), fname,
                                                    tr("Logfile (*.log *.txt)"));

    if (fileName.length())
      wnd->exportLogfile(fileName);
  }
}
