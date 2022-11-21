SET(LFA_MODELS_MOC_CLASSES
	EntryFormatingModel.h
	LogEntryFilterChain.h
	LogEntryTableFilter.h
	LogEntryTableModel.h
  StringCacheTreeModel.h
)

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

QT5_WRAP_CPP(LFA_MODELS_MOC_SOURCES ${LFA_MODELS_MOC_CLASSES})

ADD_LIBRARY(${LFA_MODELS_LIB} STATIC ${LFA_MODELS_LIB_SOURCES} ${LFA_MODELS_MOC_SOURCES})

target_link_libraries(${LFA_MODELS_LIB}
    Qt5::Gui
)
