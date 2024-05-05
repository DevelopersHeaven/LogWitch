/*
 * DummyLogSource.cpp
 *
 *  Created on: Apr 19, 2019
 *      Author: sven
 */

#include "Plugins/LogSource/dummy/DummyLogSource.h"

#include <QApplication>
#include <QMenu>

#include "ParserActionInterface.h"

#include "Plugins/LogSource/dummy/LogEntryParser_dummy.h"

using namespace logwitch::plugins::dummy;

DummyLogSource::DummyLogSource ()
{
  m_pluginDescription.name = "dummy";
  m_pluginDescription.description =
    "Plugin for testing, demo and development purposes. It will create log entries "
    "by time and also has some error propagation tests builtin.";
  m_pluginDescription.version = "1.0.0";
}

void DummyLogSource::fillMenu( QMenu* menu )
{
  menu->addSection("Dummy");

  auto actionOpenDummyLogger = menu->addAction(QApplication::translate("Plugin_Source_Dummy", "Open", nullptr));
  QObject::connect(actionOpenDummyLogger, SIGNAL(triggered()), this, SLOT(openDummyLogfile()));

  auto actionAddEntries = menu->addAction(QApplication::translate("Plugin_Source_Dummy", "Add Entries", nullptr));
  QObject::connect(actionAddEntries, SIGNAL(triggered()), this, SLOT(moreDummyLogfile()));

  m_actionErrorCreation = menu->addAction(QApplication::translate("Plugin_Source_Dummy", "Create Error", nullptr));
  m_actionErrorCreation->setCheckable(true);
  m_actionErrorCreation->setChecked(false);
  QObject::connect(actionAddEntries, SIGNAL(triggered()), this, SLOT(updateErrorEmit()));
}


void DummyLogSource::openDummyLogfile()
{
  // Create table with log entries and a new model for this
  std::shared_ptr<LogEntryParser_dummy> parser(new LogEntryParser_dummy);
  m_parser = parser;
  updateErrorEmit();

  m_parserActionIfc->newParser(parser);
}

void DummyLogSource::moreDummyLogfile()
{
  if (!m_parser)
    return;

  m_parser->addEntries(100);
}

void DummyLogSource::updateErrorEmit()
{
  if (m_parser)
    m_parser->setEmitError( m_actionErrorCreation->isChecked() );
}
