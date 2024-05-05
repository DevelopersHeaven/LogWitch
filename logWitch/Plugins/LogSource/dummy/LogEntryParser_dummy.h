/*
 * LogEntryParser_dummy.h
 *
 *  Created on: May 14, 2011
 *      Author: sven
 */

#ifndef LOGENTRYPARSER_DUMMY_H_
#define LOGENTRYPARSER_DUMMY_H_

#include <QtCore/QtCore>

#include "LogData/LogEntryParser.h"

class LogEntryParserModelConfiguration;

namespace logwitch
{
  namespace plugins
  {
    namespace dummy
    {
      /**
       * This class is a dummy for testing without a valid logfile.
       * Generates dummy entries.
       */
      class LogEntryParser_dummy : public QThread, public LogEntryParser
      {
      Q_OBJECT
      public:
        LogEntryParser_dummy ();
        ~LogEntryParser_dummy() override;

        void startEmiting() override;

        bool initParser() override;

        void addEntries (int count);

        void setEmitError( bool v) { m_emitError=v; }

        std::shared_ptr<LogEntryParserModelConfiguration> getParserModelConfiguration() const override;

        QString getName() const override
        {
          return QString("Dummylogger");
        }

      signals:
        void newEntry(TconstSharedNewLogEntryMessage) override;

        void signalError(QString error) override;

        void finished() override;

      protected:
        void run() override;

      private:
        TSharedLogEntry getNextLogEntry ();

        int m_entries;

        bool m_abort;

        int m_count;

        bool m_emitError = false;

        std::shared_ptr<LogEntryFactory> myFactory;

        std::shared_ptr<LogEntryParserModelConfiguration> m_myModelConfig;
      };
    }
  }
}
#endif /* LOGENTRYPARSER_DUMMY_H_ */
