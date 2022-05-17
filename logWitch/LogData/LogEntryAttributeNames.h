/*
 * LogEntryAttributeNames.h
 *
 *  Created on: Jun 23, 2011
 *      Author: sven
 */

#ifndef LOGENTRYATTRIBUTENAMES_H_
#define LOGENTRYATTRIBUTENAMES_H_

#include <boost/function.hpp>
#include <QtCore/QtCore>

class ImportExportDescription
{
public:
    virtual ~ImportExportDescription();
    virtual QVariant operator()(const QString &str ) = 0;
    virtual QString operator()(const QVariant &str );

    virtual const QString getImportExportDescription();
    virtual void setImportExportDescription( const QString & );

    virtual std::shared_ptr<ImportExportDescription> clone() const = 0;
};

struct ExportToQStringAdapter
{
    ExportToQStringAdapter( std::shared_ptr<ImportExportDescription> desc ):m_desc(desc){ }
    QString operator()(const QVariant &str ){ return (*m_desc)(str); };
    std::shared_ptr<ImportExportDescription> m_desc;
};

class AttributeConfiguration
{
public:
    typedef std::pair<QString,QString> TQStringPair;

    AttributeConfiguration( bool caching, int defaultCellWidth, TQStringPair names, std::shared_ptr<ImportExportDescription> factory );
    AttributeConfiguration( const AttributeConfiguration& cfg );
    AttributeConfiguration &operator=( const AttributeConfiguration& other );

    bool caching;
    int defaultCellWidth;
    TQStringPair names;

    std::shared_ptr<ImportExportDescription> attributeFactory;
};

class LogEntryAttributeNames
{
    Q_DECLARE_TR_FUNCTIONS(LogEntryAttributeNames)
public:


    LogEntryAttributeNames();

    /**
     * Returns the default configuration for the column named with columnName.
     */
    const AttributeConfiguration &getConfiguration( const QString &columnName ) const;

private:
    const AttributeConfiguration::TQStringPair attDescNumber;
    const AttributeConfiguration::TQStringPair attDescTimestamp;
    const AttributeConfiguration::TQStringPair attDescMessage;
    const AttributeConfiguration::TQStringPair attDescLoglevel;
    const AttributeConfiguration::TQStringPair attDescLoglevelNo;
    const AttributeConfiguration::TQStringPair attDescNDC;
    const AttributeConfiguration::TQStringPair attDescThread;
    const AttributeConfiguration::TQStringPair attDescThreadName;
    const AttributeConfiguration::TQStringPair attDescLogger;
    const AttributeConfiguration::TQStringPair attDescFileSource;
    const AttributeConfiguration::TQStringPair attDescFilename;
    const AttributeConfiguration::TQStringPair attDescProcess;
    const AttributeConfiguration::TQStringPair attDescProcessName;
    const AttributeConfiguration::TQStringPair attDescFunctionName;

    typedef std::map<QString, AttributeConfiguration> StringIntMap;

    StringIntMap m_defaultCellIfos;

    AttributeConfiguration m_defaultCellIfo;


};



#endif /* LOGENTRYATTRIBUTENAMES_H_ */
