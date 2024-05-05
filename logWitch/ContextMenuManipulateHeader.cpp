/*
 * ContextMenuLogEntryHeader.cpp
 *
 *  Created on: Dec 29, 2011
 *      Author: sven
 */

#include "ContextMenuManipulateHeader.h"

#include "GUITools/SlotToBoostFunction.h"
#include "GUITools/SynchronizedHeaderView.h"

ContextMenuManipulateHeader::ContextMenuManipulateHeader(QHeaderView *parent)
:QMenu(parent)
, m_header(parent)
{
  m_hideAction = addAction(tr("hide"));
  m_showMenu = addMenu(tr("show"));

  QObject::connect(m_header, &QWidget::customContextMenuRequested,
      this, &ContextMenuManipulateHeader::contextMenuRequest);

  QObject::connect(m_hideAction, &QAction::triggered, this, &ContextMenuManipulateHeader::hideColumn);
}

void ContextMenuManipulateHeader::contextMenuRequest(const QPoint & pos)
{
  m_headerToWorkOn = m_header->logicalIndexAt(pos);

  // Configure menu ....
  m_showMenu->clear();

  SynchronizedHeaderView *syncHeader =
      dynamic_cast<SynchronizedHeaderView *>(m_header);

  for (int i = 0; i < m_header->model()->columnCount(); i++)
  {
    if (m_header->isSectionHidden(i))
    {
      QVariant v = m_header->model()->headerData(i, Qt::Horizontal,
          Qt::DisplayRole);
      QString name = v.toString();
      QAction *action = m_showMenu->addAction(name);

      if (syncHeader)
      {
        connect(action, &QAction::triggered,
        // Destruction of this will be handled by the action itself.
            new SlotToBoostFunction(action,
                std::bind(&SynchronizedHeaderView::showSection, syncHeader,
                    i)), &SlotToBoostFunction::handleSignal);
      }
      else
      {
        connect(action, &QAction::triggered,
            // Destruction of this will be handled by the action itself.
            new SlotToBoostFunction(action,
                std::bind(&QHeaderView::showSection, m_header, i)),
            &SlotToBoostFunction::handleSignal);
      }
    }
  }

  m_showMenu->setEnabled(!m_showMenu->isEmpty());

  m_hideAction->setEnabled(
      m_header->hiddenSectionCount() < m_header->count() - 1);

  popup(m_header->mapToGlobal(pos));
}

void ContextMenuManipulateHeader::hideColumn()
{
  // Ensure at least one column is not hidden ....
  if (m_header->hiddenSectionCount() < m_header->count() - 1)
  {
    // This is necessary, because these methods are not virtual in QT *grr*
    SynchronizedHeaderView *syncHeader =
        dynamic_cast<SynchronizedHeaderView *>(m_header);
    if (syncHeader)
      syncHeader->hideSection(m_headerToWorkOn);
    else
      m_header->hideSection(m_headerToWorkOn);
  }
}

