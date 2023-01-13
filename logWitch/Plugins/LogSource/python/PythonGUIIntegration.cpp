/*
 * PythonGUIIntegration.cpp
 *
 *  Created on: Apr 18, 2019
 *      Author: sven
 */

#include "Plugins/LogSource/python/PythonGUIIntegration.h"

#include <QApplication>

#include <QtCore>
#include <QLabel>
#include <QSpinBox>

#include "ParserActionInterface.h"

#include "Plugins/LogSource/python/LogEntryParser_pythonSocket.h"

namespace{
  const QString settingskey_port("plugins/python/port");
}

using namespace logwitch::plugins::python;

PythonGUIIntegration::PythonGUIIntegration ()
{
  m_pluginDescription.name = "Python";
  m_pluginDescription.description =
      "Plugin supports python TCP remote socket logger.";
  m_pluginDescription.version = "1.0.0";

  QSettings settings;

  m_toolbar = new QToolBar("Python");

  QAction* actionOpenServer = new QAction(this);
  actionOpenServer->setObjectName(QStringLiteral("actionOpenPythonServer"));
  QIcon icon1;
  icon1.addFile(QStringLiteral(":/icons/networkPython"), QSize(), QIcon::Normal,
                QIcon::On);
  actionOpenServer->setIcon(icon1);

  actionOpenServer->setText(
      QApplication::translate("Plugin_Source_Python", "Start Server",
      Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
  actionOpenServer->setToolTip(
      QApplication::translate("Plugin_Source_Python",
                              "Starts the Python logging server.",
                              Q_NULLPTR));
#endif // QT_NO_TOOLTIP

  QLabel *portLabel = new QLabel(
      QApplication::translate("Plugin_Source_Python", "Port: ", Q_NULLPTR));
  m_port = new QSpinBox(m_toolbar);
  m_port->setToolTip(
      QApplication::translate("Plugin_Source_Python",
                              "Port to listen for Python incomming data.",
                              Q_NULLPTR));
  m_port->setMinimum(1);
  m_port->setMaximum(65535);
  m_port->setValue( settings.value( settingskey_port, 9020).toInt() );

  m_toolbar->addAction(actionOpenServer);
  m_toolbar->addWidget(portLabel);
  m_toolbar->addWidget(m_port);

  QObject::connect(actionOpenServer, SIGNAL(triggered()), this,
                   SLOT(openPort()));
}

PythonGUIIntegration::~PythonGUIIntegration ()
{
  QSettings settings;
  settings.setValue(settingskey_port, m_port->value() );

  delete m_toolbar;
}

void PythonGUIIntegration::openPort ()
{
  int port = m_port->value();
  std::shared_ptr<LogEntryParser_pythonSocket> socketParser(
      new LogEntryParser_pythonSocket(port));

  m_parserActionIfc->newParser(socketParser);
}

