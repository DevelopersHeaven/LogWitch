SET(LFA_LOGDATA_LIB_SOURCES
  EntryToTextFormaterDefault.cpp
  LogDataTypeRegistration.cpp
  LogEntry.cpp
  LogEntryAttributeNames.cpp
  LogEntryFactory.cpp
  LogEntryParser_Logfile.cpp
  LogEntryParser_LogfileLWI.cpp
  LogEntryParserModelConfiguration.cpp
  NewLogEntryMessage.cpp
  ObjectCache.hxx
  ParserStreamGetter.cpp
  ParserStreamGetterFile.cpp
)

ADD_LIBRARY(${LFA_LOGDATA_LIB} STATIC ${LFA_LOGDATA_LIB_SOURCES})

TARGET_LINK_LIBRARIES(${LFA_LOGDATA_LIB}
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Widgets
)

################################################################################
# TEST
IF(NOT WITHOUT_TEST)
  FIND_PACKAGE(Boost COMPONENTS unit_test_framework REQUIRED)
  
  SET(TEST_LFA_LOGDATA_LIB_SOURCES
    test/test_init.cpp
    test/LogEntryStorerForTesting.cpp
    
    test/test_LogEntryParser_Logfile.cpp
  )

  ADD_EXECUTABLE( testLogData ${TEST_LFA_LOGDATA_LIB_SOURCES})
  
  TARGET_LINK_LIBRARIES(testLogData 
      Boost::unit_test_framework
      ${LFA_LOGDATA_LIB}
  )
  
  ADD_TEST(NAME testLogData COMMAND ${CMAKE_CURRENT_BINARY_DIR}/testLogData)
ENDIF()
