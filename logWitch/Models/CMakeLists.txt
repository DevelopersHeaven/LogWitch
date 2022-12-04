SET(LFA_MODELS_LIB_SOURCES
	EntryFormatingModel.cpp
	ExportableIfc.cpp
  LogEntryFilter.cpp
  LogEntryFilterChain.cpp
  LogEntryRemoveFilter.cpp
  LogEntryTableFilter.cpp
  LogEntryTableModel.cpp
  LogEntryTableModelFileExporter.cpp
  StringCacheTreeItem.cpp
  StringCacheTreeModel.cpp
)

ADD_LIBRARY(${LFA_MODELS_LIB} STATIC ${LFA_MODELS_LIB_SOURCES})

target_link_libraries(${LFA_MODELS_LIB}
    Qt${QT_VERSION_MAJOR}::Gui
)
