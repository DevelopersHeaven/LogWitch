/*
 * LogEntryAttributeNames.cpp
 *
 *  Created on: Jun 23, 2011
 *      Author: sven
 */

#include "LogData/LogEntryAttributeNames.h"

#include <QDateTime>
#include <QDebug>
#include <QRegularExpression>
#include <QVariant>
#include <utility>

const QString ImportExportDescription::getImportExportDescription()
{
    return "";
}

void ImportExportDescription::setImportExportDescription( const QString&  )
{
}

QString ImportExportDescription::operator()(const QVariant &str )
{
    return str.toString();
}

namespace
{
    class QStringToNumber: public ImportExportDescription
    {
    public:
        QVariant operator()(const QString &str) override
        {
            bool ok = false;
            int value = str.toInt(&ok);
            if( !ok )
                return { int(0) };
            else
                return { value };
        }

        std::shared_ptr<ImportExportDescription> clone() const override
        {
            return std::make_shared<QStringToNumber>( *this );
        }
    };

    class QStringToDateTime: public ImportExportDescription
    {
    public:
        QStringToDateTime( QString fmt ): m_timeFormat(std::move( fmt )) {}

        QVariant operator()(const QString &str) override
        {
            QDateTime timestamp = QDateTime::fromString( str, m_timeFormat );
            return timestamp;
        }

        QString operator()(const QVariant &str) override
        {
            if (str.userType() == QMetaType::QDateTime)
            {
                auto date = str.value<QDateTime>();
                return date.toString(m_timeFormat);
            }
            else
            {
                return str.toString();
            }
        }

        const QString getImportExportDescription() override
        {
            return QString("DATETIME('"+m_timeFormat+"')");
        }

        void setImportExportDescription( const QString& str) override
        {
            static const QRegularExpression rx("^DATETIME\\('(.*)'\\)");
            const auto match = rx.match(str);
            if (match.hasPartialMatch())
                 m_timeFormat = match.captured(1);
            else
            {
                qDebug() << "Format string unparsable: " << str;
            }
        }

        std::shared_ptr<ImportExportDescription> clone() const override
        {
            return std::make_shared<QStringToDateTime>( *this );
        }
    private:
        QString m_timeFormat;
    };

    class QStringToVariant: public ImportExportDescription
    {
    public:
        QVariant operator()(const QString &str) override
        {
            return str;
        }

        std::shared_ptr<ImportExportDescription> clone() const override
        {
            return std::make_shared<QStringToVariant>( *this );
        }
    };
}

LogEntryAttributeNames::LogEntryAttributeNames()
:attDescNumber("number",tr("Number"))
,attDescTimestamp("timestamp",tr("Timestamp"))
,attDescMessage("message",tr("Log Message"))
,attDescLoglevel("level",tr("Loglevel"))
,attDescLoglevelNo("levelNo",tr("Loglevel #"))
,attDescNDC("ndc",tr("Nested Diagnostic content"))
,attDescThread("thread",tr("Thread"))
,attDescThreadName("threadName",tr("Thread Name"))
,attDescLogger("logger",tr("Logger"))
,attDescFileSource("fsource",tr("File Source"))
,attDescFilename("filename",tr("File Name"))
,attDescProcess("process",tr("Process"))
,attDescProcessName("processName",tr("Process Name"))
,attDescFunctionName("funcName",tr("Function Name"))
,m_defaultCellIfo( false, 150, AttributeConfiguration::TQStringPair("unknown", tr("Unknown") ), std::make_shared<QStringToVariant>() )
{
    m_defaultCellIfos.insert( StringIntMap::value_type( attDescNumber.first       , AttributeConfiguration( false, 60 , attDescNumber
            , std::make_shared<QStringToNumber>() ) ) );
    m_defaultCellIfos.insert( StringIntMap::value_type( attDescTimestamp.first    , AttributeConfiguration( false, 180, attDescTimestamp
            , std::make_shared<QStringToDateTime>("yyyy-MM-dd HH:mm:ss.zzz") ) ) );
    m_defaultCellIfos.insert( StringIntMap::value_type( attDescMessage.first      , AttributeConfiguration( false, 500, attDescMessage
            , std::make_shared<QStringToVariant>() ) ) );
    m_defaultCellIfos.insert( StringIntMap::value_type( attDescLoglevel.first     , AttributeConfiguration( true,  70 , attDescLoglevel
            , std::make_shared<QStringToVariant>() ) ) );
    m_defaultCellIfos.insert( StringIntMap::value_type( attDescLoglevelNo.first     , AttributeConfiguration( true,  70 , attDescLoglevelNo
            , std::make_shared<QStringToVariant>() ) ) );
    m_defaultCellIfos.insert( StringIntMap::value_type( attDescNDC.first          , AttributeConfiguration( true,  100, attDescNDC
            , std::make_shared<QStringToVariant>() ) ) );
    m_defaultCellIfos.insert( StringIntMap::value_type( attDescThread.first       , AttributeConfiguration( true,  70 , attDescThread
            , std::make_shared<QStringToVariant>() ) ) );
    m_defaultCellIfos.insert( StringIntMap::value_type( attDescLogger.first       , AttributeConfiguration( true,  250, attDescLogger
            , std::make_shared<QStringToVariant>() ) ) );
    m_defaultCellIfos.insert( StringIntMap::value_type( attDescFileSource.first   , AttributeConfiguration( true,  150, attDescFileSource
            , std::make_shared<QStringToVariant>() ) ) );
    m_defaultCellIfos.insert( StringIntMap::value_type( attDescFilename.first   , AttributeConfiguration( true,  100, attDescFilename
            , std::make_shared<QStringToVariant>() ) ) );
    m_defaultCellIfos.insert( StringIntMap::value_type( attDescThreadName.first   , AttributeConfiguration( true,  125, attDescThreadName
            , std::make_shared<QStringToVariant>() ) ) );
    m_defaultCellIfos.insert( StringIntMap::value_type( attDescProcess.first   , AttributeConfiguration( true,  70, attDescProcess
            , std::make_shared<QStringToVariant>() ) ) );
    m_defaultCellIfos.insert( StringIntMap::value_type( attDescProcessName.first   , AttributeConfiguration( true,  125, attDescProcessName
            , std::make_shared<QStringToVariant>() ) ) );
    m_defaultCellIfos.insert( StringIntMap::value_type( attDescFunctionName.first   , AttributeConfiguration( true,  125, attDescFunctionName
            , std::make_shared<QStringToVariant>() ) ) );
}

const AttributeConfiguration &LogEntryAttributeNames::getConfiguration( const QString &name ) const
{
    auto it = m_defaultCellIfos.find( name );
    if( it != m_defaultCellIfos.end() )
        return it->second;
    else
        return m_defaultCellIfo;
}

AttributeConfiguration::AttributeConfiguration( bool caching, int defaultCellWidth, TQStringPair namesIn,  std::shared_ptr<ImportExportDescription> factory )
: caching( caching )
, defaultCellWidth( defaultCellWidth )
, names(std::move( namesIn ))
, attributeFactory(std::move( factory ))
{
}

AttributeConfiguration::AttributeConfiguration( const AttributeConfiguration& cfg )
: caching( cfg.caching )
, defaultCellWidth( cfg.defaultCellWidth )
, names( cfg.names )
, attributeFactory( cfg.attributeFactory->clone() )
{ }

AttributeConfiguration &AttributeConfiguration::operator=( const AttributeConfiguration &other )
{
    if( this == &other)
        return *this;

    // This may fail and throw, so do this first, everything else does never throw.
    attributeFactory = other.attributeFactory->clone();

    caching = other.caching;
    defaultCellWidth = other.defaultCellWidth;
    names = other.names;

    return *this;
}
