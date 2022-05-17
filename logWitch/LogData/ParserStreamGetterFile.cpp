/*
 * ParserStreamGetterFile.cpp
 *
 *  Created on: 12 Mar 2016
 *      Author: sven
 */

#include <LogData/ParserStreamGetterFile.h>

#include <QtCore/QtCore>

namespace
{
  struct DeleterQTextStream{
    DeleterQTextStream(std::shared_ptr<QFile> file): file(file){}

    void operator()(QTextStream *obj) {
      delete obj;
    }
    std::shared_ptr<QFile> file;
  };
}

ParserStreamGetterFile::ParserStreamGetterFile(const QString &filename)
: ParserStreamGetter( QFileInfo( filename ).fileName() )
, m_filename( filename )
{
}

std::shared_ptr<QTextStream> ParserStreamGetterFile::getStream()
{
  if (!m_textStream)
  {
    std::shared_ptr<QFile> file( new QFile(m_filename) );

    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
      m_error = QObject::tr("Unable to open file for reading.");
    }
    else
    {
      m_textStream.reset( new QTextStream, DeleterQTextStream(file) );
      m_textStream->setDevice( file.get() );
    }
  }

  return m_textStream;
}
