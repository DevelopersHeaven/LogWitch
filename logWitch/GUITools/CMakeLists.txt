SET(LFA_GUITOOLS_LIB_SOURCES
	EventFilterToBoostFunction.cpp
    QScrollDownTableView.cpp
    SignalMultiplexerStateApplier.cpp
    SignalMultiplexer.cpp
    SlotToBoostFunction.h
    SynchronizedHeaderView.cpp
    WidgetStateSaver.cpp
)

ADD_LIBRARY(${LFA_GUITOOLS_LIB} STATIC ${LFA_GUITOOLS_LIB_SOURCES})

target_link_libraries(${LFA_GUITOOLS_LIB}
    Qt5::Widgets
)
