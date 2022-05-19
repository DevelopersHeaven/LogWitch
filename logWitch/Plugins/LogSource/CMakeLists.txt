# This is the other stuff needed for the plugin integration library part.
SET(LFA_PLUGINS_BASIC_LIB_MOC_CLASSES
  Interface/LogSourcePlugin.h
)

SET(LFA_PLUGINS_BASIC_LIB_SOURCES
  Interface/LogSourcePlugin.cpp
)

QT5_WRAP_CPP(LFA_PLUGINS_BASIC_LIB_MOC_SOURCES ${LFA_PLUGINS_BASIC_LIB_MOC_CLASSES})

ADD_LIBRARY(${LFA_PLUGINS_BASIC_LIB} STATIC ${LFA_PLUGINS_BASIC_LIB_SOURCES} ${LFA_PLUGINS_BASIC_LIB_MOC_SOURCES})

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
