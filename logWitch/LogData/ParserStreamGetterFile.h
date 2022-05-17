/*
 * ParserStreamGetterFile.h
 *
 *  Created on: 12 Mar 2016
 *      Author: sven
 */

#ifndef LOGWITCH_LOGDATA_PARSERSTREAMGETTERFILE_H_
#define LOGWITCH_LOGDATA_PARSERSTREAMGETTERFILE_H_

#include "LogData/ParserStreamGetter.h"

class ParserStreamGetterFile: public ParserStreamGetter
{
public:
  ParserStreamGetterFile(const QString &filename);

  virtual std::shared_ptr<QTextStream> getStream();

private:
  QString m_filename;
};

#endif /* LOGWITCH_LOGDATA_PARSERSTREAMGETTERFILE_H_ */
