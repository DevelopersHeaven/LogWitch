/*
 * ParserActionInterface.h
 *
 *  Created on: Apr 18, 2019
 *      Author: sven
 */

#ifndef LOGWITCH_PARSERACTIONINTERFACE_H_
#define LOGWITCH_PARSERACTIONINTERFACE_H_

#include <memory>

class LogEntryParser;

namespace logwitch
{
	/**
	 * Interface to signal a new parser action which requests the main application to
	 * open a new window
	 */
	class ParserActionInterface
	{
	public:
		virtual ~ParserActionInterface() = default;

		/**
		 * A new parser has been created an shall be passed to the application
		 */
		virtual void newParser(std::shared_ptr<LogEntryParser> parser, bool alreadyInitialized = false) = 0;
	};
}

#endif /* LOGWITCH_PARSERACTIONINTERFACE_H_ */
