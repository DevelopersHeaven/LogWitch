#include "logfileanalyser.h"

#include <iostream>

#include <QtGui>
#include <QApplication>

#include "LogData/LogEntry.h"
#include "LogData/NewLogEntryMessage.h"
#include "Types.h"

// This file is generated by the build system and will hold the version and some other things.
#include "BuildIfos.h"


int main(int argc, char *argv[])
{
    qRegisterMetaType<TSharedLogEntry>("TSharedLogEntry");
    qRegisterMetaType<TSharedConstQString>("TSharedConstQString");
    qRegisterMetaType<TSharedNewLogEntryMessage>("TSharedNewLogEntryMessage");
    qRegisterMetaType<TSharedNewLogEntryMessage>("TconstSharedNewLogEntryMessage");

    QCoreApplication::setOrganizationName("Steckmann");
    QCoreApplication::setOrganizationDomain("steckmann.de");
    QCoreApplication::setApplicationName("LogfileAnalyser");

    QApplication a(argc, argv);
    LogfileAnalyser w;

    w.setWindowTitle( globals::applicationName + " " + globals::applicationVersion );

    w.show();
    return a.exec();
}
