/*
 * PythonGUIIntegration.h
 *
 *  Created on: Apr 18, 2019
 *      Author: sven
 */

#ifndef LOGWITCH_PLUGINS_LOGSOURCE_PYTHON_PYTHONGUIINTEGRATION_H_
#define LOGWITCH_PLUGINS_LOGSOURCE_PYTHON_PYTHONGUIINTEGRATION_H_

#include <QtCore>
#include <QObject>
#include <QToolBar>
#include <QSpinBox>

#include "Plugins/LogSource/Interface/LogSourcePlugin.h"
#include "ParserActionInterface.h"

namespace logwitch { namespace plugins { namespace python {

/**
 * Integration part of Python to the logwitch plugin concept. This class
 * handles ToolBar and the reaction to the ToolBar.
 */
class PythonGUIIntegration: public QObject, public LogSourcePluginAbstractBase
{
	Q_OBJECT
	Q_INTERFACES(logwitch::plugins::LogSourcePlugin)
	Q_PLUGIN_METADATA(IID "de.steckmann.logwitch.plugins.LogSourcePlugin/1.0")

public:
	PythonGUIIntegration();

	virtual ~PythonGUIIntegration();

	virtual QToolBar* getToolbar() override {return m_toolbar; }

private Q_SLOTS:
	void openPort();

private:
	QToolBar* m_toolbar;
	QSpinBox* m_port;
};

}}}

#endif /* LOGWITCH_PLUGINS_LOGSOURCE_PYTHON_PYTHONGUIINTEGRATION_H_ */
