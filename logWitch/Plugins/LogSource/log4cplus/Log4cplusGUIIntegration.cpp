/*
 * Log4cplusGUIIntegration.cpp
 *
 *  Created on: Apr 18, 2019
 *      Author: sven
 */

#include "Plugins/LogSource/log4cplus/Log4cplusGUIIntegration.h"

#include <QApplication>

#include <QtCore>
#include <QLabel>

#include "Plugins/LogSource/log4cplus/LogEntryParser_log4cplusSocket.h"

namespace{
  const QString settingskey_port("plugins/log4cplus/port");
}

using namespace logwitch::plugins::log4cplus;

Log4cplusGUIIntegration::Log4cplusGUIIntegration ()
{
  m_pluginDescription.name = "Log4cplus";
  m_pluginDescription.description =
      "Plugin supports log4cplus framework with the remote logging socket appender.";
  m_pluginDescription.version = "1.0.0";

  QSettings settings;

  m_toolbar = new QToolBar("Log4cplus");

  QAction* actionOpenServer = new QAction(this);
  actionOpenServer->setObjectName(QStringLiteral("actionOpenLog4cplusServer"));
  QIcon icon1;
  icon1.addFile(QStringLiteral(":/icons/networkLog4CPlus"), QSize(), QIcon::Normal,
                QIcon::On);
  actionOpenServer->setIcon(icon1);

  actionOpenServer->setText(
      QApplication::translate("Plugin_Source_Log4cplus", "Start Server",
      Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
  actionOpenServer->setToolTip(
      QApplication::translate("Plugin_Source_Log4cplus",
                              "Starts the Log4cplus logging server.",
                              Q_NULLPTR));
#endif // QT_NO_TOOLTIP

  QLabel *portLabel = new QLabel(
      QApplication::translate("Plugin_Source_Log4cplus", "Port: ", Q_NULLPTR));
  m_port = new QSpinBox(m_toolbar);
  m_port->setToolTip(
      QApplication::translate("Plugin_Source_Log4cplus",
                              "Port to listen for log4cplus socket appender",
                              Q_NULLPTR));
  m_port->setMinimum(1);
  m_port->setMaximum(65535);
  m_port->setValue( settings.value( settingskey_port, 9998).toInt() );

  m_toolbar->addAction(actionOpenServer);
  m_toolbar->addWidget(portLabel);
  m_toolbar->addWidget(m_port);

  QObject::connect(actionOpenServer, SIGNAL(triggered()), this,
                   SLOT(openPort()));
}

Log4cplusGUIIntegration::~Log4cplusGUIIntegration ()
{
  QSettings settings;
  settings.setValue(settingskey_port, m_port->value() );

  delete m_toolbar;
}

void Log4cplusGUIIntegration::openPort ()
{
  int port = m_port->value();
  std::shared_ptr<LogEntryParser_log4cplusSocket> socketParser(
      new LogEntryParser_log4cplusSocket(port));

  m_parserActionIfc->newParser(socketParser);
}

