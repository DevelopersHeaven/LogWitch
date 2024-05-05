/*
 * DisplayItemData.h
 *
 *  Created on: Jun 10, 2011
 *      Author: sven
 */

#ifndef DISPLAYITEMDATA_H_
#define DISPLAYITEMDATA_H_
#include <QtCore/QVariant>

class DisplayItemData
{
public:
    virtual ~DisplayItemData() = default;

    /**
     * This method is called if one will display the information enclosed with this
     * action in a table or a tree for example.
     */
    virtual QVariant toDisplay( int role ) const = 0;
};

typedef std::shared_ptr<DisplayItemData> TSharedDisplayItemData;
typedef std::shared_ptr<const DisplayItemData> TconstSharedDisplayItemData;

#endif /* DISPLAYITEMDATA_H_ */
