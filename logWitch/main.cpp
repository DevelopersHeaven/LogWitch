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
#ifndef NDEBUG
    qDebug() << "WARNING: Running with debug compile options. Performance may be lower.";
#endif

    qRegisterMetaType<TSharedLogEntry>("TSharedLogEntry");
    qRegisterMetaType<TSharedConstQString>("TSharedConstQString");
    qRegisterMetaType<TSharedNewLogEntryMessage>("TSharedNewLogEntryMessage");
    qRegisterMetaType<TSharedNewLogEntryMessage>("TconstSharedNewLogEntryMessage");

    QCoreApplication::setOrganizationName("Steckmann");
    QCoreApplication::setOrganizationDomain("steckmann.de");
    QCoreApplication::setApplicationName( globals::applicationName );
    QCoreApplication::setApplicationVersion( globals::applicationVersion );

    QApplication a(argc, argv);
    LogfileAnalyser w;

    const QString mainWindowGeometry_Identifier( "MainWindowGeometry" );

    {
        QSettings settings;
        if( settings.contains( mainWindowGeometry_Identifier) )
            w.restoreGeometry( settings.value( mainWindowGeometry_Identifier).toByteArray() );

        w.setWindowTitle( globals::applicationName + " " + globals::applicationVersion );
    }

    w.show();
    int returnValue = a.exec();

    // Save window geometry
    {
        QSettings settings;
        settings.setValue( mainWindowGeometry_Identifier, w.saveGeometry() );
    }

    return returnValue;
}
