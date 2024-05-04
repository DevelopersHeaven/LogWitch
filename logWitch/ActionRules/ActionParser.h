/*
 * ActionParser.h
 *
 *  Created on: Jun 19, 2011
 *      Author: sven
 */

#ifndef ACTIONPARSER_H_
#define ACTIONPARSER_H_

#include <QtCore>

#include "ActionRules/Action.h"
#include "LogData/LogEntryParserModelConfiguration.h"

class ActionParser
{
public:
    ActionParser( TSharedConstLogEntryParserModelConfiguration cfg = TSharedConstLogEntryParserModelConfiguration());

    /**
     * Parse an action and returns true if successful.
     */
    bool parse(  const QString &expression );

    /**
     * Returns the last parsed Action
     */
    TSharedAction get() const;

    /**
     * Returns true if the last action was parsed successful.
     */
    bool isValid() const;

    /**
     * Returns a string with the description of the last parse error.
     */
    const QString &getError() const;

private:
    TSharedAction m_action;

    QString m_error;

    TSharedConstLogEntryParserModelConfiguration m_cfg;
};

#endif /* ACTIONPARSER_H_ */
