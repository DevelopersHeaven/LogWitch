/*
 * FilterListView.h
 *
 *  Created on: Jun 4, 2011
 *      Author: sven
 */

#ifndef FILTERLISTVIEW_H_
#define FILTERLISTVIEW_H_
#include <memory>

#include <QAction>
#include <QMenu>
#include <QTreeView>

class LogEntryTableWindow;
class LogEntryParserModelConfiguration;
class StringCacheTreeModel;

class FilterListView
    : public QTreeView
{
        Q_OBJECT
public:
    FilterListView( QObject *parent, std::shared_ptr<const LogEntryParserModelConfiguration> cfg, const int attr );
    ~FilterListView();

    void addToTabs( QTabWidget *tabs, LogEntryTableWindow* widget  );

public slots:
    void contextMenuPopup( const QPoint &pos);

private:
    std::shared_ptr<const LogEntryParserModelConfiguration> m_config;

    StringCacheTreeModel *m_strModel;
    int m_attr;

    QMenu *m_contextMenu;
    QAction *m_resetSelectionsAct;
    QAction *m_deselectThisAct;
    QAction *m_selectThisAct;
    QAction *m_selectOnlyThisWithPathAct;
    QAction *m_selectOnlyThisWithoutPathAct;
    QAction *m_checkTreeAct;
    QAction *m_uncheckTreeAct;
};

#endif /* FILTERLISTVIEW_H_ */
