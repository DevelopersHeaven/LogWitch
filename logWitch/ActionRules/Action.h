/*
 * Action.h
 *
 *  Created on: Jun 5, 2011
 *      Author: sven
 */

#ifndef ACTION_H_
#define ACTION_H_
#include <memory>
#include <QtCore/QVariant>

#include "ActionRules/DisplayItemData.h"

/**
 * This is triggered by a specific incoming log message or new connection.
 */
class Action
    : public DisplayItemData
{
public:
    /**
     * With this the data row can be modified.
     *
     * @param var incoming original data to modify or leave.
     * @param column number of the column the data is from
     * @param role the role which this var is for.
     * @return returns true if var was modified, otherwise false.
     */
    virtual bool modifyData( QVariant &var,  int column, int role) const;

    /**
     * Returns true if the compiled action is valid within the given context.
     * An invalid action can be displayed, but not executed in this context.
     */
    virtual bool isValid() const;
};

typedef std::shared_ptr<Action> TSharedAction;
typedef std::shared_ptr<const Action> TconstSharedAction;


#endif /* ACTION_H_ */
