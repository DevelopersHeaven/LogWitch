/*
 * LogEntryStreamGetterIF.h
 *
 *  Created on: 12 Mar 2016
 *      Author: sven
 */

#ifndef LOGWITCH_LOGDATA_LOGENTRYSTREAMGETTER_H_
#define LOGWITCH_LOGDATA_LOGENTRYSTREAMGETTER_H_

/**
 * This is a getter interface for information and streams needed for log file
 * parsing.
 */
class LogEntryStreamGetter
{
public:
  LogEntryStreamGetter( const QString name );

  virtual ~LogEntryStreamGetter();

  /**
   * Returns a stream if initialization was possible or an empty pointer
   * if something went wrong. If there is something wring, getError can be used
   * to retrieve an error message.
   */
  std::shared_ptr<QTextStream> getStream() = 0;

  QString getError() const { return m_error; }

  QString getName() const { return m_name; }

protected:
  QString m_name;
  QString m_error;
};

#endif /* LOGWITCH_LOGDATA_LOGENTRYSTREAMGETTER_H_ */
