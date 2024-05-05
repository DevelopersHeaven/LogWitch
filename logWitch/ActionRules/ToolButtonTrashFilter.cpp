/*
 * ToolButtonTrashFilter.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: sven
 */

#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

#include "ActionRules/ToolButtonTrashFilter.h"
#include "ActionRules/TableModelRulesCompiled.h"

ToolButtonTrashFilter::ToolButtonTrashFilter( QAction * action, TableModelRulesCompiled *model )
: QToolButton( )
, m_model( model )
{
    setDefaultAction( action );
    setAcceptDrops(true);
}

void ToolButtonTrashFilter::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "ToolButtonTrashFilter::dragEnterEvent";
    if (event->mimeData()->hasFormat( TableModelRulesCompiled::ruleMimeType ) )
        event->acceptProposedAction();
}

void ToolButtonTrashFilter::dropEvent(QDropEvent *event)
{
    qDebug()<<"ToolButtonTrashFilter::dropEvent";
    if (event->mimeData()->hasFormat( TableModelRulesCompiled::ruleMimeType ) )
    {
        event->accept();
        event->setDropAction(Qt::MoveAction);
        m_model->removeRules( event->mimeData() );
    }
    else
    {
        event->ignore();
    }
}
