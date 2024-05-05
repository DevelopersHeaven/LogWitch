/*
 * test_StringCacheTreeItem.cpp
 *
 *  Created on: May 1, 2012
 *      Author: sven
 */

#define BOOST_TEST_DYN_LINK

#define BOOST_TEST_MODULE StringCacheTreeItem
#include <boost/test/unit_test.hpp>

#include "Models/StringCacheTreeItem.h"

/**
 * This test is testing if adding items works well.
 */
BOOST_AUTO_TEST_CASE( addItems )
{
    StringCacheTreeItem root(std::make_shared<const QString>("Root")
            , std::make_shared<const QString>("Root"));

    BOOST_CHECK_EQUAL( root.childCount(), 0);

    root.appendChild( new StringCacheTreeItem(
              std::make_shared<const QString>("Test")
            , std::make_shared<const QString>("Test")));

    BOOST_CHECK_EQUAL( root.childCount(), 1);

    root.appendChild( new StringCacheTreeItem(
              std::make_shared<const QString>("Test2")
            , std::make_shared<const QString>("Test2")));

    BOOST_CHECK_EQUAL( root.childCount(), 2);
}

BOOST_AUTO_TEST_CASE( checkItems )
{
    StringCacheTreeItem root(std::make_shared<const QString>("Root")
            , std::make_shared<const QString>("Root"));

    StringCacheTreeItem *RootA = new StringCacheTreeItem(
              std::make_shared<const QString>("A")
            , std::make_shared<const QString>("Root.A")) ;
    root.appendChild( RootA );

    StringCacheTreeItem *RootB = new StringCacheTreeItem(
              std::make_shared<const QString>("B")
            , std::make_shared<const QString>("Root.B")) ;
    root.appendChild( RootB );

    StringCacheTreeItem *RootA1 = new StringCacheTreeItem(
              std::make_shared<const QString>("1")
            , std::make_shared<const QString>("Root.A.1")) ;
    RootA->appendChild( RootA1 );

    // Structure building finished. Do consistency check
    BOOST_CHECK_EQUAL( root.childCount(), 2 );
    BOOST_CHECK_EQUAL( RootA->childCount(), 1 );
    BOOST_CHECK_EQUAL( RootA1->childCount(), 0 );
    BOOST_CHECK_EQUAL( RootB->childCount(), 0 );

    BOOST_CHECK_EQUAL( root.getChecked(), Qt::Checked );
    BOOST_CHECK_EQUAL( RootA->getChecked(), Qt::Checked );
    BOOST_CHECK_EQUAL( RootA1->getChecked(), Qt::Checked );
    BOOST_CHECK_EQUAL( RootB->getChecked(), Qt::Checked );



}

