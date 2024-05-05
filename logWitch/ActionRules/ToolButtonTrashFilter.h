/*
 * ToolButtonTrashFilter.h
 *
 *  Created on: Jan 4, 2012
 *      Author: sven
 */

#ifndef TOOLBUTTONTRASHFILTER_H_
#define TOOLBUTTONTRASHFILTER_H_

#include <QToolButton>

class TableModelRulesCompiled;

/**
 * This class is responsible for displaying a toolbar button which supports
 * dropping a rule on it to delete it.
 */
class ToolButtonTrashFilter
: public QToolButton
{
    Q_OBJECT
public:
    ToolButtonTrashFilter( QAction * action, TableModelRulesCompiled *model );

    void dropEvent(QDropEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);

private:
    TableModelRulesCompiled *m_model;
};

#endif /* TOOLBUTTONTRASHFILTER_H_ */
