CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12 FATAL_ERROR)

PROJECT(LogWitch)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FIND_PROGRAM(LSB_RELEASE_CMD lsb_release)
FIND_PACKAGE(Git)

ADD_CUSTOM_TARGET(getversion ALL
	DEPENDS VersionFile )

ADD_CUSTOM_COMMAND(OUTPUT VersionFile ${CMAKE_CURRENT_BINARY_DIR}/Version.cmake
 	COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                    		-P ${CMAKE_CURRENT_SOURCE_DIR}/getgitversion.cmake)

IF( NOT EXISTS  ${CMAKE_CURRENT_BINARY_DIR}/version.cmake )
	file(COPY version.cmake DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	INCLUDE( getgitversion.cmake )
ENDIF()

INCLUDE( ${CMAKE_CURRENT_BINARY_DIR}/version.cmake )

SET (VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD})
MESSAGE(STATUS "LogWitch ${VERSION} built with prefix: ${CMAKE_INSTALL_PREFIX}")

# Set default value for the build type to release, if nothing else was specified
IF(NOT CMAKE_BUILD_TYPE)
	MESSAGE(STATUS "No build type selected, default to Release.\n"
                       "Use cmake -DCMAKE_BUILD_TYPE=debug to build a debug build")
	SET(CMAKE_BUILD_TYPE "Release")
endif()

# List of candidates for the Qt framework to build against. CMake will pick the
# highest version available. Override this variable on the command line to
# specify an exact Qt version, e.g. `-DUSE_QT=Qt5`.
set(USE_QT Qt5 Qt6 CACHE STRING "Qt framework to build against")

OPTION (WITH_PLUGINLOG4CPLUS "Compile log4cplus plugin" ON)
OPTION (WITH_PLUGINPYTHON "Compile python plugin" ON)
OPTION (WITH_PLUGINDUMMY "Compile dummy plugin" ON)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

### Definitions of Directories.
SET(MAIN_DIR_NAME "LogWitch")

SET(HELPDIR "share/${MAIN_DIR_NAME}")
SET(HELPDIRABSOLUTE "${CMAKE_INSTALL_PREFIX}/${HELPDIR}")

SET(DOCDIR "share/doc/logwitch")

IF(UNIX)
	INSTALL( FILES ${CMAKE_SOURCE_DIR}/debian/copyright DESTINATION ${DOCDIR} ) 
ELSE()
	SET(PACKAGING_ADDDIR ${CMAKE_SOURCE_DIR}/packaging/windows)
ENDIF()

### Search dependencies

find_package(QT NAMES ${USE_QT} REQUIRED)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  find_package(Qt${QT_VERSION_MAJOR} 6.3 COMPONENTS Concurrent Core Help Network ToolsTools Widgets REQUIRED)
else()
  if (WIN32)
    message(FATAL_ERROR "Building on Windows requires Qt >= 6")
  endif()
  find_package(Qt${QT_VERSION_MAJOR} 5.12 COMPONENTS Concurrent Core Help Network Widgets REQUIRED)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

FIND_PACKAGE(Boost REQUIRED)

set(CMAKE_CXX_STANDARD 17)
if(UNIX OR MINGW)
	set(CMAKE_CXX_FLAGS_RELEASE        "-pipe -O2    -fvisibility-inlines-hidden -fdiagnostics-show-option -fno-strict-aliasing -Wall -Werror -Wsuggest-override -Wl,--as-needed -fPIC -DNDEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG          "-pipe -O0 -g -fvisibility-inlines-hidden -fdiagnostics-show-option -Wall -Werror -Wsuggest-override -Wl,--as-needed -fPIC")
	if(MINGW)
		set(CMAKE_CXX_FLAGS            "-Wno-error=attributes -Wno-error=deprecated-declarations")
	endif()
else()
	set(CMAKE_CXX_FLAGS_RELEASE        "/O2 --Zc:wchar_t /wd4100 /wd4002 /wd4101 /wd4541 /wd4065 /wd4996 /wd4244 /GR /bigobj /MP -MD -DNDEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG          "/Od /ZI -Zc:wchar_t /wd4100 /wd4002 /wd4101 /wd4541 /wd4065 /wd4996 /wd4244 /GR /bigobj /MP -MDd")
	set (CMAKE_SHARED_LINKER_FLAGS 	   "${CMAKE_SHARED_LINKER_FLAGS} /LARGEADDRESSAWARE")
	set (CMAKE_EXE_LINKER_FLAGS        "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
endif()

SET(LFA_RULES_LIB "logfileanalyzer_rules_lib")
SET(LFA_GUITOOLS_LIB "logfileanalyzer_guitools_lib")
SET(LFA_HELP_LIB "logfileanalyzer_help_lib")
SET(LFA_LOGDATA_LIB "logfileanalyzer_logdata_lib")
SET(LFA_MODELS_LIB "logfileanalyzer_models_lib")
SET(LFA_PLUGINS_BASIC_LIB "logfileanalyzer_plugins_basic_lib")

SET(LFA_PLUGIN_LOG4CPLUS_LIB "logwitch_plugin_log4cplus")
SET(LFA_PLUGIN_DUMMY_LIB "logwitch_plugin_dummy")
SET(LFA_PLUGIN_PYTHON_LIB "logwitch_plugin_python")

IF(NOT WITHOUT_TEST)
  ENABLE_TESTING()
ENDIF()

SET(LW_PLUGIN_DIR lib/logwitch)
SET(LW_BINARY_DIR bin)

# This directory is relative to bin to ensure install in custom destinations will work. 
SET(LW_BIN_TO_PLUGIN_DIR ../lib/logwitch)



ADD_SUBDIRECTORY(logWitch)

### Testing stuff
IF(NOT WITHOUT_TEST)
	ADD_SUBDIRECTORY(test)
ENDIF()


######################## CPack information if CPack is available.
IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
  SET(CPACK_STRIP_FILES TRUE)
  SET(CPACK_DEBIAN_PACKAGE_NAME "LogWitch")
  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Analyzer for log files") 
  SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Analyzer for log files\n Analyzer can be used to receive log files from the a log4cpp service\n and show them.")
  SET(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
  SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
  SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_BUILD}")
  SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sven Steckmann <logwitch@steckmann.de>")
  SET(CPACK_PACKAGE_VENDOR "Sven Steckmann") 
  SET(CPACK_PACKAGE_CONTACT "logwitch@steckmann.de")
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "LogWitch ${VERSION}")
  SET(CPACK_PACKAGE_EXECUTABLES "LogWitch")
  SET(CPACK_GENERATOR "TGZ")
  
  FIND_PROGRAM(LSB_RELEASE_CMD lsb_release)
  IF(NOT TARGET_SYSTEM)
    IF(NOT LSB_RELEASE_CMD)
      MESSAGE(WARNING "DEB Generator: Can't find lsb_release in your path. Setting TARGET_SYSTEM manually.")
    ELSE()
      EXECUTE_PROCESS(COMMAND ${LSB_RELEASE_CMD} -cs
                    OUTPUT_VARIABLE TARGET_SYSTEM
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    ENDIF()
  ENDIF()
  
  SET(CPACK_DEBIAN_PACKAGE_SECTION "devel")

  IF(NOT TARGET_SYSTEM)
    IF(NOT LSB_RELEASE_CMD)
      message(WARNING "DEB Generator: Can't find lsb_release in your path. Not setting TARGET_SYSTEM")
    ELSE()
      EXECUTE_PROCESS(COMMAND ${LSB_RELEASE_CMD} -cs
                      OUTPUT_VARIABLE TARGET_SYSTEM
                      OUTPUT_STRIP_TRAILING_WHITESPACE)
    ENDIF()
  ENDIF()

  IF(TARGET_SYSTEM STREQUAL "bionic")
    MESSAGE(STATUS "Using library versions for Ubuntu 18.04")
	IF(USELOG4CPLUSV2)
      SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, log4cplus (>= 2.0), libqt5core5a:amd64 (>=5.9), libqt5gui5:amd64 (>=5.9), libqt5network5:amd64 (>=5.9), libqt5help5 (>=5.9)")
	ELSE()
      SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, liblog4cplus-1.1-9 (>= 1.1), libqt5core5a:amd64 (>=5.9), libqt5gui5:amd64 (>=5.9), libqt5network5:amd64 (>=5.9), libqt5help5 (>=5.9)")
	ENDIF()
  ELSE()
	if(UNIX)
    	message( WARNING "Unable to determine distribution version: ${TARGET_SYSTEM}" )
	endif(UNIX)
	
  ENDIF()

  # generate a Debian compliant changelog
  SET(CHANGELOG_HEADER "${APP_NAME_LC} (${CPACK_DEBIAN_PACKAGE_VERSION}) ${TARGET_SYSTEM}\; urgency=high")
  EXECUTE_PROCESS(COMMAND date -R OUTPUT_VARIABLE RFC2822_TIMESTAMP)
  # two spaces between maintainer and timestamp is NOT a mistake
  SET(CHANGELOG_FOOTER " -- ${CPACK_DEBIAN_PACKAGE_MAINTAINER}  ${RFC2822_TIMESTAMP}")

#  find_program(GZIP_CMD gzip)
#  if(GIT_FOUND AND GZIP_CMD AND EXISTS ${CMAKE_SOURCE_DIR}/.git)
#    execute_process(COMMAND ${GIT_EXECUTABLE} log --no-merges --pretty=format:"%n  [%an]%n   * %s" --since="30 days ago"
#                    OUTPUT_VARIABLE CHANGELOG
#                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#                    OUTPUT_STRIP_TRAILING_WHITESPACE)
#    string(REPLACE "\"" "" CHANGELOG ${CHANGELOG})
#    file(WRITE ${CPACK_PACKAGE_DIRECTORY}/deb/changelog.Debian ${CHANGELOG_HEADER}\n${CHANGELOG}\n\n${CHANGELOG_FOOTER})
#    execute_process(COMMAND ${GZIP_CMD} -f -9 -n ${CPACK_PACKAGE_DIRECTORY}/deb/changelog.Debian)
#    unset(CHANGELOG_HEADER)
#    unset(CHANGELOG_FOOTER)
#    unset(RFC2822_TIMESTAMP)
#  endif()

  INCLUDE(CPack)
ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
