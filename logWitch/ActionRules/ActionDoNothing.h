/*
 * ActionDoNothing.h
 *
 *  Created on: Jun 25, 2011
 *      Author: sven
 */

#ifndef ACTIONDONOTHING_H_
#define ACTIONDONOTHING_H_
#include "ActionRules/Action.h"

class ActionDoNothing
: public Action
{
public:
    QVariant toDisplay( int role ) const;
};

#endif /* ACTIONDONOTHING_H_ */
