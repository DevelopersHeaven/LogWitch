/*
 * DummyLogSource.h
 *
 *  Created on: Apr 19, 2019
 *      Author: sven
 */

#ifndef LOGWITCH_PLUGINS_LOGSOURCE_DUMMY_DUMMYLOGSOURCE_H_
#define LOGWITCH_PLUGINS_LOGSOURCE_DUMMY_DUMMYLOGSOURCE_H_
#include <memory>

#include <QObject>

#include "Plugins/LogSource/Interface/LogSourcePlugin.h"

class QAction;

namespace logwitch
{
  namespace plugins
  {
    namespace dummy
    {
      class LogEntryParser_dummy;

      /**
       * This source is for debugging, developing, demonstration and testing.
       * It just creates log entries by time and also creates errors and shows
       * how they were propagated to the user.
       */
      class DummyLogSource: public QObject, public LogSourcePluginAbstractBase
      {
        Q_OBJECT
        Q_INTERFACES(logwitch::plugins::LogSourcePlugin)
        Q_PLUGIN_METADATA(IID "de.steckmann.logwitch.plugins.LogSourcePlugin/1.0")

      public:
        DummyLogSource ();

        void fillMenu( QMenu* menu ) override;

      private slots:
        void openDummyLogfile();

        void moreDummyLogfile();

        void updateErrorEmit();

      private:
        // This is the last created dummy logfile parser
        std::shared_ptr<LogEntryParser_dummy> m_parser;

        QAction* m_actionErrorCreation;
      };

    }
  }
}

#endif /* LOGWITCH_PLUGINS_LOGSOURCE_DUMMY_DUMMYLOGSOURCE_H_ */
