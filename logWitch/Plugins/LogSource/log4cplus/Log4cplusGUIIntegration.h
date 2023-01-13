/*
 * Log4cplusGUIIntegration.h
 *
 *  Created on: Apr 18, 2019
 *      Author: sven
 */

#ifndef LOGWITCH_PLUGINS_LOGSOURCE_LOG4CPLUS_LOG4CPLUSGUIINTEGRATION_H_
#define LOGWITCH_PLUGINS_LOGSOURCE_LOG4CPLUS_LOG4CPLUSGUIINTEGRATION_H_

#include <QtCore>
#include <QObject>

#include "Plugins/LogSource/Interface/LogSourcePlugin.h"

class QSpinBox;
class QToolBar;

namespace logwitch { namespace plugins { namespace log4cplus {

/**
 * Integration part of log4cplus to the logwitch plugin concept. This class
 * handles ToolBar and the reaction to the ToolBar.
 */
class Log4cplusGUIIntegration: public QObject, public LogSourcePluginAbstractBase
{
	Q_OBJECT
	Q_INTERFACES(logwitch::plugins::LogSourcePlugin)
	Q_PLUGIN_METADATA(IID "de.steckmann.logwitch.plugins.LogSourcePlugin/1.0")

public:
	Log4cplusGUIIntegration();

	virtual ~Log4cplusGUIIntegration();

	virtual QToolBar* getToolbar() override {return m_toolbar; }

private slots:
	void openPort();

private:
	QToolBar* m_toolbar;
	QSpinBox* m_port;
};

}}}

#endif /* LOGWITCH_PLUGINS_LOGSOURCE_LOG4CPLUS_LOG4CPLUSGUIINTEGRATION_H_ */
