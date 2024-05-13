/*
 * CompiledRulesStateSaver.h
 *
 *  Created on: Jun 12, 2011
 *      Author: sven
 */

#ifndef COMPILEDRULESSTATESAVER_H_
#define COMPILEDRULESSTATESAVER_H_

#include <QTableView>

#include "ActionRules/TableModelRulesCompiled.h"
#include "LogData/LogEntryParserModelConfiguration.h"

class FilterRuleSelectionWindow;

class CompiledRulesStateSaver
: public QObject
{
    Q_OBJECT
public:
    CompiledRulesStateSaver(  TSharedConstLogEntryParserModelConfiguration cfg, TSharedRuleTable ruleTable );
    ~CompiledRulesStateSaver();

    void connectActions( FilterRuleSelectionWindow *wnd );

    QWidget *m_displayWidget;

    QTableView *m_compiledRuleView;
    TableModelRulesCompiled *m_rulesCompiledModel;

private:
    bool m_connected = false;
    QAction *m_removeSelectedRules;
};

typedef std::shared_ptr<CompiledRulesStateSaver> TSharedCompiledRulesStateSaver;
typedef std::shared_ptr<const CompiledRulesStateSaver> TconstCompiledRulesStateSaver;

#endif /* COMPILEDRULESSTATESAVER_H_ */
