SET(LFA_GUITOOLS_MOC_CLASSES
	QScrollDownTableView.h
	SignalMultiplexer.h
	SlotToBoostFunction.h
	SynchronizedHeaderView.h
	EventFilterToBoostFunction.h
)

SET(LFA_GUITOOLS_LIB_SOURCES
	EventFilterToBoostFunction.cpp
    QScrollDownTableView.cpp
    SignalMultiplexerStateApplier.cpp
    SignalMultiplexer.cpp
    SynchronizedHeaderView.cpp
    WidgetStateSaver.cpp
)

IF(USEQT5)
  QT5_WRAP_CPP(LFA_GUITOOLS_MOC_SOURCES ${LFA_GUITOOLS_MOC_CLASSES})
ELSE()
  QT4_WRAP_CPP(LFA_GUITOOLS_MOC_SOURCES ${LFA_GUITOOLS_MOC_CLASSES})
ENDIF()

ADD_LIBRARY(${LFA_GUITOOLS_LIB} STATIC ${LFA_GUITOOLS_LIB_SOURCES} ${LFA_GUITOOLS_MOC_SOURCES})
