/*
 * LogEntryTableModelFileExporter.h
 *
 *  Created on: 26 Oct 2013
 *      Author: sven
 */

#ifndef LOGENTRYTABLEMODELFILEEXPORTER_H_
#define LOGENTRYTABLEMODELFILEEXPORTER_H_
#include <QtCore>

#include "LogData/LogEntry.h"

class LogEntryTableModel;


/**
 * This is a exporter class which can be used to export the stuff of a LogEntryTableModel
 * to a file which can be used to reconstruct the model afterwards.
 */
class LogEntryTableModelFileExporter
{
public:
    LogEntryTableModelFileExporter( LogEntryTableModel& model );

    virtual ~LogEntryTableModelFileExporter();

    void exportTo( const QString& filename );

    void exportTo( const QString& filename, std::function<TconstSharedLogEntry()> funcNextExportItem );
private:
    LogEntryTableModel& m_model;
};

#endif /* LOGENTRYTABLEMODELFILEEXPORTER_H_ */
