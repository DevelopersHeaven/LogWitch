/*
 * QScrollDownTableView.h
 *
 *  Created on: Jun 26, 2011
 *      Author: sven
 */

#ifndef QSCROLLDOWNTABLEVIEW_H_
#define QSCROLLDOWNTABLEVIEW_H_
#include <QTableView>

class QScrollDownTableView
:public QTableView
{
    Q_OBJECT
public:
    QScrollDownTableView(QWidget *parent = nullptr);

protected:
    void updateGeometries() override;
};

#endif /* QSCROLLDOWNTABLEVIEW_H_ */
