#include <LogData/test/LogEntryStorerForTesting.h>
#include "LogData/LogEntryParser_Logfile.h"

#include "LogData/ParserStreamGetter.h"
/*
 * test_init.cpp
 *
 *  Created on: 12 Mar 2016
 *      Author: sven
 */
#include <QVariant>

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "LogEntryStorerForTesting.h"
#include "LogData/LogEntry.h"

using namespace std::placeholders;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  #define endl Qt::endl
#endif

void fillStream(QTextStream& ts, bool withEndl=false, bool withMultilineMessage=false)
{
  ts << "2016-03-08 15:23:23,904 - INFO - Mix.Dist.Command - [/Test.cpp:143:executeCommand] - Received command" << endl;
  if (withMultilineMessage) {
    ts << " More message data 1" << endl;
  }
  ////////
  ts << "2016-03-08 15:23:23,904 - INFO - Mix.Dist.Mode - [/Mode.cpp:313:exit] - Exiting Mode " << endl;

  ////////
  ts << "2016-03-08 15:23:23,908 - INFO - Mix.Dist.Control - [/Control.cpp:113:handleExit] - handleExit" << endl;
  if (withMultilineMessage) {
    ts << " More message data 2" << endl;
  }

  ////////
  ts << "2016-03-08 15:23:23,909 - INFO - Mix.Dist.Control - [/Control.cpp:65:shutdown] - Control shutdown";
  if (withMultilineMessage) {
    ts << endl << " More message data 3" ;
  }

  if (withEndl)
    ts << endl;
}

/**
 * Performs the test and returns the testing object for final inspection.
 */
std::unique_ptr<LogEntryStorerForTesting> performTest(
      std::function<void(QTextStream&)> prepare )
{
  QString s;
  std::shared_ptr<QTextStream> textStream( new QTextStream(&s) );
  prepare(*textStream);

  std::shared_ptr<ParserStreamGetter> getter( new ParserStreamGetter("name", textStream) );
  std::unique_ptr<LogEntryParser> parser( new LogEntryParser_Logfile( getter ) );

  std::unique_ptr<LogEntryStorerForTesting> tester( new LogEntryStorerForTesting( parser.get() ) );
  tester->start();
  return tester;
}

BOOST_AUTO_TEST_CASE( bugLastLogLineWithoutEndl )
{
  const auto tester = performTest(std::bind(&fillStream, _1, false, false));
  BOOST_CHECK( tester->m_finished );

  BOOST_CHECK_EQUAL( tester->m_entries.size(), 4 );
  BOOST_CHECK_EQUAL( tester->m_errors.size(), 0 );
}

BOOST_AUTO_TEST_CASE( bugLastLogLineWithEndl )
{
  const auto tester = performTest(std::bind(&fillStream, _1, true, false));
  BOOST_CHECK( tester->m_finished );

  BOOST_CHECK_EQUAL( tester->m_entries.size(), 4 );
  BOOST_CHECK_EQUAL( tester->m_errors.size(), 0 );
}

BOOST_AUTO_TEST_CASE( bugLastLogLineWithoutEndlMultiLine )
{
  const auto tester = performTest(std::bind(&fillStream, _1, false, true));
  BOOST_CHECK( tester->m_finished );

  BOOST_CHECK_EQUAL( tester->m_entries.size(), 4 );
  BOOST_CHECK_EQUAL( tester->m_errors.size(), 0 );
}

BOOST_AUTO_TEST_CASE( bugLastLogLineWithEndlMultiLine )
{
  const auto tester = performTest(std::bind(&fillStream, _1, true, true));
  BOOST_CHECK( tester->m_finished );

  BOOST_CHECK_EQUAL( tester->m_entries.size(), 4 );
  BOOST_CHECK_EQUAL( tester->m_errors.size(), 0 );
}

void filStreamWithIncrementingMessage( QTextStream& ts, int count, bool multiline )
{
  for (int i = 0; i < count; ++i ) {
    ts << "2016-03-08 15:23:23,904 - INFO - Mix.Dist.Command - [/Test.cpp:143:executeCommand] - " << i << "XX"<< endl;
    if (multiline) {
      for( int j = 0; j < 3; ++j)
        ts << " sdfg sdf asdf asdf asdf asdf asdf asd fas df asdf asdf asdf asdf asdf asdf asdf asdf asd fasd fasd f" << endl;
    }
  }
}

BOOST_AUTO_TEST_CASE( multiThreadedParsing )
{
  const auto tester = performTest(std::bind(&filStreamWithIncrementingMessage, _1, 20000, false));
  BOOST_CHECK( tester->m_finished );

  int i = 0;
  for (const TSharedLogEntry& entry : tester->m_entries)
  {
    QString s;
    QTextStream ts(&s);
    ts << i << "XX";
    ts.flush();

    BOOST_CHECK_EQUAL( entry->getAttribute(0).value<int>(), i+1 );
    BOOST_CHECK( entry->getAttribute(2).value<TSharedConstQString>()->startsWith(s) );

    // qDebug() << entry->getAttribute(0) << "MSG" << (* entry->getAttribute(2).value<TSharedConstQString>());
    ++i;
  }

  BOOST_CHECK_EQUAL( tester->m_entries.size(), 20000 );
  BOOST_CHECK_EQUAL( tester->m_errors.size(), 0 );
}

BOOST_AUTO_TEST_CASE( multiThreadedParsingMultiline )
{
  const auto tester = performTest(std::bind(&filStreamWithIncrementingMessage, _1, 30000, true));
  BOOST_CHECK( tester->m_finished );

  int i = 0;
  for (const TSharedLogEntry& entry : tester->m_entries)
  {
    QString s;
    QTextStream ts(&s);
    ts << i << "XX";
    ts.flush();

    BOOST_CHECK_EQUAL( entry->getAttribute(0).value<int>(), i+1 );
    BOOST_CHECK( entry->getAttribute(2).value<TSharedConstQString>()->startsWith(s) );

    int linecount = 0;
    QString sCnt(*entry->getAttribute(2).value<TSharedConstQString>());
    QTextStream in( &sCnt );
    while( !in.atEnd()) {
        in.readLine();
        linecount++;
    }

    BOOST_CHECK_EQUAL( linecount, 4 );

    // qDebug() << entry->getAttribute(0) << "MSG" << (* entry->getAttribute(2).value<TSharedConstQString>());
    ++i;
  }

  BOOST_CHECK_EQUAL( tester->m_entries.size(), 30000 );
  BOOST_CHECK_EQUAL( tester->m_errors.size(), 0 );
}

