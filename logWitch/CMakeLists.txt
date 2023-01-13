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
  	Qt${QT_VERSION_MAJOR}::Network
)

TARGET_COMPILE_DEFINITIONS( logwitch PRIVATE -DLW_PLUGIN_DIR="${LW_BIN_TO_PLUGIN_DIR}" )

###################### Installation
install (TARGETS logwitch
         RUNTIME DESTINATION "${LW_BINARY_DIR}" )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_generate_deploy_app_script(
        TARGET logwitch
        FILENAME_VARIABLE deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()
