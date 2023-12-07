/*
 * StringCacheTreeItem.cpp
 *
 *  Created on: May 15, 2011
 *      Author: sven
 */

#include "StringCacheTreeItem.h"

#include <algorithm>

StringCacheTreeItem::StringCacheTreeItem( TSharedConstQString originalString, TSharedConstQString str, StringCacheTreeItem *parent /* = NULL */ )
	: m_parentItem( parent )
	, m_checkedSelf( Inherit )
    , m_checkedChilds( Inherit )
	, m_string( str )
	, m_stringOriginal( originalString )
{

}

StringCacheTreeItem::~StringCacheTreeItem()
{
	qDeleteAll(m_childItems);
}

void StringCacheTreeItem::appendChild( StringCacheTreeItem *child)
{
	m_childItems.push_back( child );

}

StringCacheTreeItem* StringCacheTreeItem::child(int row)
{
	return m_childItems.at(row);
}

int StringCacheTreeItem::childCount() const
{
	return m_childItems.size();
}

int StringCacheTreeItem::row() const
{
    if (m_parentItem)
    {
        int idx = 0;
        TItemVector::const_iterator it = std::find( m_parentItem->m_childItems.begin(), m_parentItem->m_childItems.end(), this );
        if( it != m_parentItem->m_childItems.end() )
            idx = it - m_parentItem->m_childItems.begin();

        return idx;
    }

    return 0;
}

StringCacheTreeItem *StringCacheTreeItem::parent()
{
	return m_parentItem;
}

TSharedConstQString StringCacheTreeItem::getString() const
{
	return m_string;
}

TSharedConstQString StringCacheTreeItem::getOriginalString() const
{
	return m_stringOriginal;
}

void StringCacheTreeItem::setOriginalString( TSharedConstQString str)
{
	m_stringOriginal = str;
}

StringCacheTreeItem::CheckState StringCacheTreeItem::getCheckState( ) const
{
    CheckState stateParentInherited;
    if( this->m_checkedSelf == Unchecked )
        stateParentInherited.checked = false;

    // first determine inherited state ...
    const StringCacheTreeItem *item = NULL;
    for( item = this->m_parentItem; item != NULL; item = item->m_parentItem )
    {
        if( item->m_checkedChilds == Checked )
        {
            stateParentInherited.checked = true;
            stateParentInherited.forced = true;
        }
        else if( item->m_checkedChilds == Unchecked )
        {
            stateParentInherited.checked = false;
            stateParentInherited.forced = true;
        }
    }

    // goto children if not Forced ...
    if( stateParentInherited.forced  || m_checkedChilds != Inherit )
        return stateParentInherited;

    getCheckState( stateParentInherited, this );

    return stateParentInherited;
}

StringCacheTreeItem::CheckState &StringCacheTreeItem::getCheckState( StringCacheTreeItem::CheckState &state, const StringCacheTreeItem *item ) const
 {
     if( !(state.forced || state.partial) )
     {
         TItemVector::const_iterator it= item->m_childItems.begin();
         for( ;it != item->m_childItems.end(); ++it )
         {

             if( (*it)->m_checkedSelf == Unchecked
                     || ( (*it)->m_checkedChilds == Unchecked && (*it)->m_childItems.size() > 0 )  )
             {
                 state.partial = true;
             }
             else
             {
                 if( (*it)->m_checkedChilds == Inherit )
                     getCheckState( state, *it );
             }
         }
     }

     return state;
 }

Qt::CheckState StringCacheTreeItem::getChecked() const
{
    CheckState c = getCheckState();

    if( c.partial )
        return Qt::PartiallyChecked;
    else if( c.checked )
        return Qt::Checked;
    else
        return Qt::Unchecked;
}

void StringCacheTreeItem::nextChecked()
{
    CheckState state = getCheckState();

    if( state.forced )
        return;

    if( m_checkedSelf == Checked )
    {
        m_checkedSelf = Unchecked;
    }
    else if( m_checkedSelf == Unchecked )
    {
        m_checkedSelf = Inherit;
    }
    else if( m_checkedSelf == Inherit )
    {
        if( state.partial )
            m_checkedSelf = Checked;
        else
            m_checkedSelf = Unchecked;
    }

    m_checkedChilds = m_checkedSelf;
}

void StringCacheTreeItem::recursiveSetTree( Check c )
{
    recursiveSetTree( c, c );
}

void StringCacheTreeItem::recursiveSetTree( Check self, Check child )
{
    m_checkedChilds = child;
    m_checkedSelf = self;
    TItemVector::const_iterator it= m_childItems.begin();
    for( ;it != m_childItems.end(); ++it )
        (*it)->recursiveSetTree( self, child );
}

StringCacheTreeItem *StringCacheTreeItem::getRootElement()
{
    if( m_parentItem != NULL )
        return m_parentItem->getRootElement();
    else
        return this;
}

void StringCacheTreeItem::checkOnlyThisElement( bool includeTreepath, Check child, Check self )
{
    // Move upwards and uncheck everything except the tree to this element.
    if( m_parentItem != NULL )
    {
        m_checkedChilds = child;
        m_checkedSelf = self;

        TItemVector::const_iterator it = m_parentItem->m_childItems.begin();
        for( ;it != m_parentItem->m_childItems.end(); ++it )
        {
            if( *it != this )
            {
                (*it)->m_checkedChilds = (*it)->m_checkedSelf = Unchecked;
            }
        }
        m_parentItem->checkOnlyThisElement( includeTreepath, Inherit, includeTreepath ? Inherit : Unchecked );
    }
}


