/*
 * RulesTableView.h
 *
 *  Created on: Jun 25, 2011
 *      Author: sven
 */

#ifndef RULESTABLEVIEW_H_
#define RULESTABLEVIEW_H_

#include <QtGui>
#include <QTableView>

class RulesTableView
: public QTableView
{
    Q_OBJECT
public:
    RulesTableView( QWidget *parent = NULL );

};

#endif /* RULESTABLEVIEW_H_ */
