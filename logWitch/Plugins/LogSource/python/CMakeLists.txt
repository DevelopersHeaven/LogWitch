SET(Python_ADDITIONAL_VERSIONS 3)
FIND_PACKAGE(PythonInterp REQUIRED)
SET(PYTHON_VERSION_TAG "${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}")

FIND_PACKAGE(Boost COMPONENTS "python${PYTHON_VERSION_TAG}")
IF(NOT Boost_FOUND)
  FIND_PACKAGE(Boost REQUIRED COMPONENTS "python-py${PYTHON_VERSION_TAG}")
ENDIF()

FIND_PACKAGE(PythonLibs 3 REQUIRED)

message(STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
message(STATUS "PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE}")
message(STATUS "PYTHON_INCLUDE_DIRS = ${PYTHON_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}")

SET(LFA_PLUGIN_PYTHON_MOC_CLASSES
	PythonGUIIntegration.h
	LogEntryParser_pythonSocket.h
)

SET(LFA_PLUGIN_PYTHON_LIB_SOURCES
  PythonGUIIntegration.cpp
  LogEntryParser_pythonSocket.cpp
)

IF(USEQT5)
  QT5_WRAP_CPP(LFA_PLUGIN_PYTHON_MOC_SOURCES ${LFA_PLUGIN_PYTHON_MOC_CLASSES})
ELSE()
  QT4_WRAP_CPP(LFA_PLUGIN_PYTHON_MOC_SOURCES ${LFA_PLUGIN_PYTHON_MOC_CLASSES})
ENDIF()

ADD_LIBRARY(${LFA_PLUGIN_PYTHON_LIB} SHARED ${LFA_PLUGIN_PYTHON_LIB_SOURCES} ${LFA_PLUGIN_PYTHON_MOC_SOURCES})

TARGET_INCLUDE_DIRECTORIES(${LFA_PLUGIN_PYTHON_LIB} PUBLIC ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})


TARGET_LINK_LIBRARIES( ${LFA_PLUGIN_PYTHON_LIB}
  ${LFA_PLUGINS_BASIC_LIB}
  ${Boost_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${Qt5Network_LIBRARIES}
)

TARGET_COMPILE_DEFINITIONS(${LFA_PLUGIN_PYTHON_LIB} PRIVATE -DQT_PLUGIN)
TARGET_COMPILE_DEFINITIONS(${LFA_PLUGIN_PYTHON_LIB} PRIVATE -DQT_SHARED)
TARGET_COMPILE_DEFINITIONS(${LFA_PLUGIN_PYTHON_LIB} PRIVATE -DQT_NO_KEYWORDS)

install (TARGETS ${LFA_PLUGIN_PYTHON_LIB}
         LIBRARY DESTINATION "${LW_PLUGIN_DIR}" )