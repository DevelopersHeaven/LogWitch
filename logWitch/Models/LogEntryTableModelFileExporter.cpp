/*
 * LogEntryTableModelFileExporter.cpp
 *
 *  Created on: 26 Oct 2013
 *      Author: sven
 */

#include "Models/LogEntryTableModelFileExporter.h"

#include "LogData/LogEntryFactory.h"

#include "Models/LogEntryTableModel.h"

LogEntryTableModelFileExporter::LogEntryTableModelFileExporter( LogEntryTableModel& model )
: m_model( model )
{
}

namespace
{
    const QString timestampFormat("yyyy-MM-dd hh:mm:ss.zzz");

    struct ExportFormater
    {
        QString operator()(const QVariant &value)
        {
            switch(value.userType())
            {
            case QMetaType::QDateTime:
                return value.value<QDateTime>().toString(timestampFormat);
            default:
              ;
            }

            return value.toString();
        }
    };

    struct DumpAllEntries
    {
        DumpAllEntries( LogEntryTableModel& model )
        : m_model( model)
        , m_entriesInModel(m_model.rowCount())
        , m_entryId(0)
        {}

        TconstSharedLogEntry operator()()
        {
            if (m_entryId<m_entriesInModel)
                return m_model.getEntryByRow(m_entryId++);
            else
                return TconstSharedLogEntry();
        }

        LogEntryTableModel& m_model;
        const int m_entriesInModel;
        int m_entryId;
    };
}

void LogEntryTableModelFileExporter::exportTo( const QString& filename )
{
    exportTo( filename, DumpAllEntries(m_model) );
}

void LogEntryTableModelFileExporter::exportTo( const QString& filename, std::function<TconstSharedLogEntry()> funcNextExportItem )
{
    std::any lock = m_model.getLock();

    QFile file( filename );
    file.open(QIODevice::WriteOnly);
    QTextStream str( &file );

    TSharedConstLogEntryParserModelConfiguration modelConfiguration = m_model.getParserModelConfiguration();

    // Determine sort order, we will put the message to the end.
    const int fields = modelConfiguration->getLogEntryFactory()->getNumberOfFields();

    // Try to find message field, we will put this to the end of the file!
    LogEntryAttributeNames names;
    int messageID = modelConfiguration->getLogEntryFactory()->getNumberOfFields() - 1;
    for( int i = 0; i < modelConfiguration->getLogEntryFactory()->getNumberOfFields(); ++i )
    {
        if( modelConfiguration->getLogEntryFactory()->getDescShort(i) == "message" )
        {
            messageID = i;
            break;
        }
    }


    std::vector<int> order;
    for ( int i = 0; i < fields; i++ )
    {
        if( i != messageID )
            order.push_back( i );
    }
    order.push_back( messageID );

    QString desc;

    for ( int i = 0; i < int( order.size() ); i++ )
    {
        if( desc.length() )
            desc.append(" - ");

        desc.append( modelConfiguration->getLogEntryFactory()->getDescShort( order[i] ) );
        desc += "(" + QString::number(order[i]) +")";

        QString imExExtension( modelConfiguration->getLogEntryFactory()->getFieldConfiguration(i).attributeFactory->getImportExportDescription() );
        if( imExExtension.length() > 0 )
        {
            desc.append( ':' );
            desc.append( imExExtension );
        }
    }

    str << "%%LWI_DESC="<< desc << "\n";
    str << "%%LWI_CFGContext="<< modelConfiguration->getConfigurationString() << "\n";

    static const QRegularExpression regexLineEnd("(\r\n|\r|\n)");
    static const QRegularExpression regexSplitter("( - )");

    for( TconstSharedLogEntry logEntry = funcNextExportItem(); logEntry; logEntry = funcNextExportItem() )
    {
        QString line;
        for ( int i = 0; i < int( order.size() ); i++ )
        {
            if( line.length() )
                line.append(" - ");

            QString entry( *logEntry->getAttributeAsString(order[i]
                         , ExportToQStringAdapter( modelConfiguration->getLogEntryFactory()->getFieldConfiguration(i).attributeFactory ) ) );
            entry.replace(regexLineEnd,"\n " );

            if( i != int( order.size()-1 ) )
              entry.replace(regexSplitter," -- " );

            line.append( entry );
        }
        str << line << "\n";
    }
}
