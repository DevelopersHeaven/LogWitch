/*
 * ActionDoNothing.cpp
 *
 *  Created on: Jun 25, 2011
 *      Author: sven
 */

#include "ActionDoNothing.h"

QVariant ActionDoNothing::toDisplay( int role ) const
{
    QVariant variant;

    if( role == Qt::DisplayRole )
    {
        variant = QString( QObject::tr("NOP") );
    }

    return variant;
}
