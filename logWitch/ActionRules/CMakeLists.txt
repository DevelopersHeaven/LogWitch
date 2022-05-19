SET(LFA_RULES_MOC_CLASSES
	CompiledRulesStateSaver.h
	FilterRuleCompiled.h
	FilterRuleSelectionWindow.h
	RuleTable.h
	RulesTableView.h
	TableModelRulesCompiled.h
	ToolButtonTrashFilter.h
)

SET(LFA_RULES_LIB_SOURCES
    Action.cpp
    ActionDataRewriter.cpp
    ActionDiscardRow.cpp
    ActionDoNothing.cpp
    ActionParser.cpp
    CompiledRulesStateSaver.cpp
    Expression.cpp
	ExpressionFind.cpp
    ExpressionMatch.cpp
    ExpressionOperators.cpp
    ExpressionParser.cpp
    ExpressionRegEx.cpp
    FilterRuleCompiled.cpp
    FilterRuleSelectionWindow.cpp
    Rule.cpp
    RuleTable.cpp
    RulesTableView.cpp
    TableModelRulesCompiled.cpp
    ToolButtonTrashFilter.cpp
    ValueGetterConstQString.cpp
    ValueGetterLogEntry.cpp
    ValueGetter.cpp
)

QT5_WRAP_CPP(LFA_RULES_MOC_SOURCES ${LFA_RULES_MOC_CLASSES})

ADD_LIBRARY(${LFA_RULES_LIB} STATIC ${LFA_RULES_LIB_SOURCES} ${LFA_RULES_MOC_SOURCES})
