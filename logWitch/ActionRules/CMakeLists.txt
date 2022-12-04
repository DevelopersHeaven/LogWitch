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

ADD_LIBRARY(${LFA_RULES_LIB} STATIC ${LFA_RULES_LIB_SOURCES})

target_link_libraries(${LFA_RULES_LIB}
    Qt${QT_VERSION_MAJOR}::Widgets
)
