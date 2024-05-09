/*
 * LogEntryParserModelConfiguration.cpp
 *
 *  Created on: May 18, 2011
 *      Author: sven
 */

#include "LogData/LogEntryParserModelConfiguration.h"

#include <LogData/EntryToTextFormaterDefault.h>
#include <QDebug>
#include <QSettings>
#include <algorithm>
#include <limits>
#include <utility>

#include "LogData/LogEntryFactory.h"

using namespace std::placeholders;

LogEntryParserModelConfiguration::LogEntryParserModelConfiguration( QString configurationString, std::shared_ptr<LogEntryFactory> factory )
    : m_formater( new EntryToTextFormaterDefault )
    , m_attr(std::move( factory ))
    , m_configurationString(std::move( configurationString ))
    , m_fieldWidthHintsLoaded( false )
    , m_fieldShowHintLoaded( false )
    , m_fieldOrderHintLoaded(false )
{
    restoreHintsFromSettings();
}

QString LogEntryParserModelConfiguration::getHierarchySplitString( int idx ) const
{
    if( idx >= 0 && idx < m_hierarchySplitstrings.size() )
        return m_hierarchySplitstrings[idx];
    else
        return "";
}

void LogEntryParserModelConfiguration::setHierarchySplitString( int idx, const QString & fmt)
{
    if( idx >= m_hierarchySplitstrings.size() )
        m_hierarchySplitstrings.resize(idx+1);

    m_hierarchySplitstrings[idx] = fmt;

    saveHintsToSettings();
}


int LogEntryParserModelConfiguration::getFieldWidthHint( int idx ) const
{
    if( idx < (int)m_fieldWidthHints.size() )
        return m_fieldWidthHints[idx] < 0 ? 150 : m_fieldWidthHints[idx];
    else
        return 150;
}

void LogEntryParserModelConfiguration::setFieldWidthHint( int idx, int width, bool def )
{
    if( m_fieldWidthHintsLoaded && def )
        return;

    while( idx >= (int)m_fieldWidthHints.size() )
        m_fieldWidthHints.push_back(-1);

    m_fieldWidthHints[idx] = width;

    saveHintsToSettings();
}

bool LogEntryParserModelConfiguration::getFieldShowHint( int idx ) const
{
    if( idx < (int)m_fieldShowHint.size() )
        return m_fieldShowHint[idx];
    else
        return true;
}

void LogEntryParserModelConfiguration::setFieldShowHint( int idx, bool show, bool def )
{
    if( m_fieldShowHintLoaded && def )
        return;

    while( idx >= (int)m_fieldShowHint.size() )
        m_fieldShowHint.push_back(true);

    m_fieldShowHint[idx] = show;

    saveHintsToSettings();
}

int LogEntryParserModelConfiguration::getFieldOrderHint( int idx) const
{
    return (idx < m_fieldOrderHint.size() && idx >= 0) ? m_fieldOrderHint[idx] : std::numeric_limits<int>::max();
}

void LogEntryParserModelConfiguration::setFieldOrderHint( const QVector<int> &in, bool def )
{
    if( m_fieldOrderHintLoaded && def )
        return;

    m_fieldOrderHint = in;

    saveHintsToSettings();
}

void LogEntryParserModelConfiguration::setFieldOrderHint( int idx, int value, bool def )
{
    if( m_fieldOrderHintLoaded && def )
        return;

    while( idx >= (int)m_fieldOrderHint.size() )
        m_fieldOrderHint.push_back(true);

    qDebug() << " set field order hint: idx " << idx << " value " << value;
    m_fieldOrderHint[idx] = value;

    saveHintsToSettings();
}

namespace
{
    template<class T>
    void convertToAndPushBack( QVector<T> *l, QVariant &v )
    {
        T val = v.value<T>();
        l->push_back( val );
    }
}

void LogEntryParserModelConfiguration::restoreHintsFromSettings()
{
    QSettings settings;

    settings.beginGroup( "ModelConfigurations");
    settings.beginGroup( m_configurationString );
    settings.beginGroup( m_attr->getDescShortAsLongSring() );

    if( settings.contains("Widths") )
    {
        QList<QVariant> vList = settings.value( "Widths" ).value< QList<QVariant> >();

        m_fieldWidthHints.clear();
        std::for_each( vList.begin(), vList.end(),
                std::bind( &convertToAndPushBack<int>, &m_fieldWidthHints, _1 ) );

        m_fieldWidthHintsLoaded = true;
    }

    if( settings.contains("Show") )
    {
        QList<QVariant> vList = settings.value( "Show" ).value< QList<QVariant> >();

        m_fieldShowHint.clear();
        std::for_each( vList.begin(), vList.end(),
                std::bind( &convertToAndPushBack<bool>, &m_fieldShowHint, _1 ) );

        m_fieldShowHintLoaded = true;
    }

    if( settings.contains("Order") )
    {
        QList<QVariant> vList = settings.value( "Order" ).value< QList<QVariant> >();

        m_fieldOrderHint.clear();
        std::for_each( vList.begin(), vList.end(),
                std::bind( &convertToAndPushBack<int>, &m_fieldOrderHint, _1 ) );

        m_fieldOrderHintLoaded = true;
    }

    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}

void LogEntryParserModelConfiguration::saveHintsToSettings() const
{
    QSettings settings;

    settings.beginGroup( "ModelConfigurations");
    settings.beginGroup( m_configurationString );
    settings.beginGroup( m_attr->getDescShortAsLongSring() );

    QList<QVariant> vList;

    for (const auto& hint : m_fieldWidthHints) {
        vList.push_back(hint);
    }

    settings.setValue( "Widths", vList );

    vList.clear();
    for (const auto& hint : m_fieldShowHint) {
        vList.push_back(hint);
    }

    settings.setValue( "Show", vList );

    vList.clear();
    for (const auto& hint : m_fieldOrderHint) {
        vList.push_back(hint);
    }

    settings.setValue( "Order", vList );

    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}

const QString &LogEntryParserModelConfiguration::getConfigurationString() const
{
    return m_configurationString;
}
