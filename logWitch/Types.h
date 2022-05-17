/*
 * Types.h
 *
 *  Created on: May 15, 2011
 *      Author: sven
 */

#ifndef TYPES_H_
#define TYPES_H_
#include <memory>

#include <QMetaType>
#include <QString>

class QString;
class QVariant;

typedef std::shared_ptr<QString> TSharedQString;
typedef std::shared_ptr<const QString> TSharedConstQString;

typedef std::shared_ptr<QVariant> TSharedQVariant;
typedef std::shared_ptr<const QVariant> TSharedConstQVariant;

Q_DECLARE_METATYPE(TSharedQString)
Q_DECLARE_METATYPE ( TSharedConstQString );

#endif /* TYPES_H_ */
