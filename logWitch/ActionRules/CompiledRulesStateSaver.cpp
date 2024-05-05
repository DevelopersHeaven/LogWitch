/*
 * CompiledRulesStateSaver.cpp
 *
 *  Created on: Jun 12, 2011
 *      Author: sven
 */

#include "ActionRules/CompiledRulesStateSaver.h"

#include <QAction>
#include <QHeaderView>
#include <QToolBar>
#include <QVBoxLayout>

#include "ActionRules/FilterRuleSelectionWindow.h"
#include "ActionRules/TableModelRulesCompiled.h"
#include "ActionRules/ToolButtonTrashFilter.h"

#include "GUITools/EventFilterToBoostFunction.h"

#include "ContextMenuManipulateHeader.h"

using namespace std::placeholders;

CompiledRulesStateSaver::CompiledRulesStateSaver(  TSharedConstLogEntryParserModelConfiguration cfg, TSharedRuleTable ruleTable )
    : m_compiledRuleView(nullptr)
    , m_connected( false)
{
   m_displayWidget = new QWidget(); //This is the pane

   QVBoxLayout* vbox = new QVBoxLayout(m_displayWidget);
   m_displayWidget->setLayout(vbox);

   QToolBar* toolBar = new QToolBar( m_compiledRuleView);
   toolBar->setIconSize(QSize(16,16));
   vbox->addWidget(toolBar);

   // Construct view and set viewing stuff
   m_compiledRuleView = new QTableView(m_displayWidget );
   m_compiledRuleView->verticalHeader()->setDefaultSectionSize( 20 );
   m_compiledRuleView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   m_compiledRuleView->horizontalHeader()->setDefaultSectionSize( 190 );

   m_compiledRuleView->setSelectionBehavior(QAbstractItemView::SelectRows);
   m_compiledRuleView->setSelectionMode( QAbstractItemView::ExtendedSelection );

   m_compiledRuleView->setDragEnabled( true );
   m_compiledRuleView->setDropIndicatorShown(true);
   m_compiledRuleView->setDragDropMode( QAbstractItemView::DragDrop );

   // Set Model
   m_rulesCompiledModel = new TableModelRulesCompiled( m_compiledRuleView, cfg, ruleTable );
   m_compiledRuleView->setModel( m_rulesCompiledModel );

   vbox->addWidget(m_compiledRuleView);

   m_removeSelectedRules = new QAction("Trash", this);
   m_removeSelectedRules->setIcon(QIcon(":/icons/trash"));
   toolBar->addWidget( new ToolButtonTrashFilter( m_removeSelectedRules, m_rulesCompiledModel ) );
}

void CompiledRulesStateSaver::connectActions( FilterRuleSelectionWindow *wnd )
{
    if( m_connected )
        return;

    QObject::connect(m_removeSelectedRules, &QAction::triggered,
            wnd, &FilterRuleSelectionWindow::removeSelectionFromCompiled);

    m_compiledRuleView->installEventFilter( new EventFilterToBoostFunction( this,
            std::bind( &evtFunc::keyPressed, _1, _2, Qt::Key_Delete,
                    std::function< void(void ) >( std::bind( &FilterRuleSelectionWindow::removeSelectionFromCompiled, wnd ) ) ) ) );

    wnd->tieHeaderChangesTo( m_compiledRuleView );

    m_compiledRuleView->horizontalHeader()->setContextMenuPolicy( Qt::CustomContextMenu );
    new ContextMenuManipulateHeader( m_compiledRuleView->horizontalHeader() );

    m_connected = true;
}

CompiledRulesStateSaver::~CompiledRulesStateSaver()
{
    delete m_displayWidget;
}
