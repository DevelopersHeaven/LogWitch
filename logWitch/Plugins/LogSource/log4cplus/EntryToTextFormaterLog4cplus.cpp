/*
 * EntryToTextFormaterLog4cplus.cpp
 *
 *  Created on: Jun 2, 2011
 *      Author: sven
 */

#include "Plugins/LogSource/log4cplus/EntryToTextFormaterLog4cplus.h"

#include <QTextStream>

#include "LogData/LogEntry.h"
#include "LogData/LogEntryFactory.h"

using namespace logwitch::plugins::log4cplus;

QString EntryToTextFormaterLog4cplus::formatEntry (
    TconstSharedLogEntry entry) const
{
  QString out;
  QTextStream str(&out);
  QString tmp;

  // retrieve the attributes from the factory ...
  if (entry->getFactory().getNumberOfFields() >= 8)
  {
    str << "<b>Timestamp:</b> " << *entry->getAttributeAsString(1) << "<br/>";
    str << "<b>Loglevel:</b> " << *entry->getAttributeAsString(3) << "<br/>";
    str << "<b>Component:</b> " << *entry->getAttributeAsString(4) << "<br/>";
    str << "<b>File source:</b> " << *entry->getAttributeAsString(5) << "<br/>";
    str << "<b>Thread:</b> " << *entry->getAttributeAsString(6) << "<br/>";
    str << "<b>Nested diagnostic content:</b> "
        << *entry->getAttributeAsString(7) << "<br/>";
    tmp = *entry->getAttributeAsString(2);
    str << "<b>Message:</b><p> <pre> "
        << tmp.toHtmlEscaped().replace("\n", "<br/>") << "</pre></p>";
  }
  else
    str
        << "<b>The attribute entry type seems to be incorrect! Please check.</b><br/>";

  return out;
}
