# This is the other stuff needed for the plugin integration library part.

SET(LFA_PLUGINS_BASIC_LIB_SOURCES
  Interface/LogSourcePlugin.cpp
)

ADD_LIBRARY(${LFA_PLUGINS_BASIC_LIB} STATIC ${LFA_PLUGINS_BASIC_LIB_SOURCES})

target_link_libraries(${LFA_PLUGINS_BASIC_LIB}
    ${LFA_LOGDATA_LIB}
    Qt${QT_VERSION_MAJOR}::Widgets
)

################################################################################
# Add plugins now
IF(WITH_PLUGINLOG4CPLUS)
  ADD_SUBDIRECTORY(log4cplus)
ENDIF()

IF(WITH_PLUGINDUMMY)
  ADD_SUBDIRECTORY(dummy)
ENDIF()

IF(WITH_PLUGINPYTHON)
  ADD_SUBDIRECTORY(python)
ENDIF()
