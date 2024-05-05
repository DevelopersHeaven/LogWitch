/*
 * StringCacheTreeItem.h
 *
 *  Created on: May 15, 2011
 *      Author: sven
 */

#ifndef STRINGCACHETREEITEM_H_
#define STRINGCACHETREEITEM_H_

#include "Types.h"
#include <QtCore/QtCore>

/**
 * This class is an element within a tree which represents a path like scheme
 * based on a string representation. For this, the class has two members, the
 * original one (this is the complete path and a shared_ptr to the string) and
 * the subpart representing the actual level only.
 *
 * Additionally the class supports setting different states, which can be used to
 * determine the "checked state" for the item based on the complete hierarchy.
 *
 */
class StringCacheTreeItem {
public:
    /**
     * Enum which defines some check states. This will be used to
     * define the state for this element.
     */
	enum Check
	{
	    Inherit, // Inherit from parent or checked if top
		Checked,
		Unchecked
	};

	class CheckState
	{
	public:
	    CheckState() : forced( false ), checked( true ), partial(false ) { }

	    bool forced;
	    bool checked;
	    bool partial;
	};

public:
	/**
	 * Constructs new object with the parameters.
	 * @param originalString This is the string with the complete hierarchy as got from logger.
	 * @param str This is the part to display inside this level.
	 * @param parent The parent node, if there is one.
	 */
	StringCacheTreeItem(TSharedConstQString originalString, TSharedConstQString str, StringCacheTreeItem *parent = nullptr);

	virtual ~StringCacheTreeItem();

    /**
     * Appends a child to the tree and adds it to the end of the list
     * of all children.
     */
    void appendChild(StringCacheTreeItem *child);

    /**
     * Returns the child of this element at index n.
     * @param n Index to return.
     * @preCondition 0 <= n < childCount()
     */
    StringCacheTreeItem *child(int n);

    /**
     * The numbers of children owned by this node.
     */
    int childCount() const;


    /**
     * Returns my position inside the parents nodes list.
     * If there is no parent node, returns 0.
     */
    int row() const;

    /**
     * Returns the parent node.
     */
    StringCacheTreeItem *parent();

    /**
     * This is the readable name which need not be the same as the string from the StringCache
     */
    TSharedConstQString getString() const;

    /**
     * This function returns the original string from the StringCache which can be used for a direct pointer
     */
    TSharedConstQString getOriginalString() const ;

    void setOriginalString( TSharedConstQString str);

    /**
     * Returns the checked state for this node to display for Qt.
     */
    Qt::CheckState getChecked() const;

    Check getCheckedSelf( ) { return m_checkedSelf; }
    void setCheckedSelf( Check c ) { m_checkedSelf = c; }
    Check getCheckedChild( ) { return m_checkedChilds; }

    CheckState getCheckState( ) const;

    /**
     * Toggles the state of this element and sets the next
     * state (works on checkedSelf state).
     */
    void nextChecked();

    /**
     * Sets the self state to this element and the child to
     * all child elements.
     * @param self State for this element
     * @param child State to set on all children
     */
    void recursiveSetTree( Check self, Check child );

    /**
     * Sets value as state for this element and all child elements.
     * @param value State for this and all child elements.
     */
    void recursiveSetTree( Check value );

    /**
     * Returns the root element of the tree (or this object if there
     * is no further root element).
     * @return Root element of this tree.
     */
    StringCacheTreeItem *getRootElement();

    void checkOnlyThisElement( bool includeTreepath = true, Check child = Checked, Check self=Checked);
private:
    CheckState &getCheckState( CheckState &state, const StringCacheTreeItem *item ) const;

private:
    typedef std::vector< StringCacheTreeItem *> TItemVector;
    TItemVector m_childItems;
    StringCacheTreeItem *m_parentItem;

    /**
     * This represents the check state for this item only.
     */
    Check m_checkedSelf;

    /**
     * This represents the check state for all child item only.
     */
    Check m_checkedChilds;

    TSharedConstQString m_string;

    TSharedConstQString m_stringOriginal;
};

#endif /* STRINGCACHETREEITEM_H_ */
