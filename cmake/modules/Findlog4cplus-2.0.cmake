# - Try to find log4cplus
# Once done, this will define
#
#  LOG4CPLUS_FOUND - system has log4cplus
#  LOG4CPLUS_INCLUDE_DIRS - the log4cplus include directories
#  LOG4CPLUS_LIBRARIES - link these to use log4cplus

find_package(PkgConfig)
include(FindPackageHandleStandardArgs)

# Use pkg-config to get hints about paths
#libfind_pkg_check_modules(LOG4CPLUS_PKGCONF LOG4CPLUS)

# Include dir
find_path(LOG4CPLUS_INCLUDE_DIR
  HINTS ${LOG4CPLUS_ROOT_DIR}/include
  PATHS usr/local/include
  NAMES log4cplus/logger.h
)

# Finally the library itself
find_library(LOG4CPLUS_LIBRARY
  NAMES log4cplus
  HINTS ${LOG4CPLUS_ROOT_DIR}/lib
)

IF( NOT UNIX)
	GET_FILENAME_COMPONENT(LOG4CPLUS_DLL_PATH ${LOG4CPLUS_LIBRARY} PATH)
	find_file(LOG4CPLUS_DLL
 	 NAMES liblog4cplus.dll log4cplus.dll
 	 PATHS ${LOG4CPLUS_DLL_PATH} ${LOG4CPLUS_ROOT_DIR}/bin
	)
ENDIF()

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(LOG4CPLUS_LIBRARIES ${LOG4CPLUS_LIBRARY} )
set(LOG4CPLUS_INCLUDE_DIRS ${LOG4CPLUS_INCLUDE_DIR} )
set(LOG4CPLUS_VERSION 2.0)

# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LOG4CPLUS  DEFAULT_MSG
                                  LOG4CPLUS_LIBRARY LOG4CPLUS_INCLUDE_DIR)
