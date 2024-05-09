/*
 * ParserStreamGetter.cpp
 *
 *  Created on: 12 Mar 2016
 *      Author: sven
 */

#include "LogData/ParserStreamGetter.h"

#include <utility>

ParserStreamGetter::ParserStreamGetter(  const QString name, std::shared_ptr<QTextStream> textStream )
: m_name( name )
, m_textStream(std::move( textStream ))
{
}
