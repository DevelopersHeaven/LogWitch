CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/GenerateIfo.h.in ${CMAKE_CURRENT_BINARY_DIR}/BuildIfos.h @ONLY)


SET(LFA_FORMS
  logfileanalyser.ui
  GUI/DialogExportOptions.ui
)

SET(LFA_SOURCES
	ContextMenuManipulateHeader.cpp
	FilterListView.cpp
	LogEntryTableWindow.cpp
	logfileanalyser.cpp
	ParserActionInterface.cpp
	main.cpp	
	
	GUI/DialogExportOptions.cpp
	GUI/QuickSearchBar.cpp
)

SET(LFA_RESOURCES 
	LogfileAnalyserResources.qrc
)

INCLUDE_DIRECTORIES(
  ${LOG4CPLUS_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}  # Neede for moc files in out of source builds.
)

#Add source dirs
ADD_SUBDIRECTORY(ActionRules)
ADD_SUBDIRECTORY(GUITools)
ADD_SUBDIRECTORY(Help)
ADD_SUBDIRECTORY(LogData)
ADD_SUBDIRECTORY(Models)
ADD_SUBDIRECTORY(Plugins)

#LINK_DIRECTORIES(
#  ${CMAKE_CURRENT_BINARY_DIR}/ActionRules
#  ${CMAKE_CURRENT_BINARY_DIR}/GUITools
#  ${CMAKE_CURRENT_BINARY_DIR}/LogData
#  ${CMAKE_CURRENT_BINARY_DIR}/Models
#)

ADD_EXECUTABLE(logwitch WIN32
	${LFA_SOURCES} 
    ${LFA_FORMS_HEADERS} 
    ${LFA_RESOURCES}
)
    
TARGET_LINK_LIBRARIES( logwitch 
  	${LFA_RULES_LIB}
  	${LFA_GUITOOLS_LIB}
  	${LFA_HELP_LIB}
   	${LFA_LOGDATA_LIB}
  	${LFA_MODELS_LIB}
)

TARGET_COMPILE_DEFINITIONS( logwitch PRIVATE -DLW_PLUGIN_DIR="${LW_BIN_TO_PLUGIN_DIR}" )

###################### Installation
install (TARGETS logwitch
         RUNTIME DESTINATION "${LW_BINARY_DIR}" )
         
IF(NOT UNIX)
	INSTALL( FILES ${QT_LIBRARY_DIR}/QtCore4.dll
		${QT_LIBRARY_DIR}/QtGUI4.dll
		${QT_LIBRARY_DIR}/QtNetwork4.dll
		${QT_LIBRARY_DIR}/QtSvg4.dll
		${QT_LIBRARY_DIR}/QtXml4.dll
		${PACKAGING_ADDDIR}/qt.conf
		DESTINATION "${LW_BINARY_DIR}" )
	INSTALL( FILES ${QT_PLUGINS_DIR}/imageformats/qsvg4.dll
		DESTINATION bin/plugins/imageformats )
ENDIF()
