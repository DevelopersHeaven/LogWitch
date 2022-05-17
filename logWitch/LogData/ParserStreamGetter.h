/*
 * ParserStreamGetter.h
 *
 *  Created on: 12 Mar 2016
 *      Author: sven
 */

#ifndef LOGWITCH_LOGDATA_PARSERSTREAMGETTER_H_
#define LOGWITCH_LOGDATA_PARSERSTREAMGETTER_H_

#include <memory>

#include <QtCore/qobject.h>
#include <QTextStream>

/**
 * This is a getter interface for information and streams needed for log file
 * parsing.
 */
class ParserStreamGetter
{
public:
  ParserStreamGetter( const QString name, std::shared_ptr<QTextStream> textStream = std::shared_ptr<QTextStream>() );

  virtual ~ParserStreamGetter();

  /**
   * Returns a stream if initialization was possible or an empty pointer
   * if something went wrong. If there is something wring, getError can be used
   * to retrieve an error message.
   */
  virtual std::shared_ptr<QTextStream> getStream() {return m_textStream; }

  /**
   * In case of errors, this is a human readable string with the error reason.
   */
  QString getError() const { return m_error; }

  /**
   * This is a name of a file, object name or something similar to make it
   * readable to the user.
   */
  QString getName() const { return m_name; }

protected:
  QString m_name;

  QString m_error;

  std::shared_ptr<QTextStream> m_textStream;
};


#endif /* LOGWITCH_LOGDATA_PARSERSTREAMGETTER_H_ */
