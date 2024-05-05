/*
 * QuickSearchBar.cpp
 *
 *  Created on: 24 Feb 2017
 *      Author: sven
 */

#include "GUI/QuickSearchBar.h"

#include <sstream>

#include <QDebug>
#include <QDragEnterEvent>
#include <QHBoxLayout>
#include <QMimeData>
#include <QPushButton>

#include "ActionRules/ActionParser.h"
#include "ActionRules/ExpressionFind.h"
#include "ActionRules/ExpressionParser.h"
#include "ActionRules/ExpressionRegEx.h"
#include "ActionRules/Rule.h"
#include "ActionRules/ValueGetterLogEntry.h"

#include "Models/LogEntryTableModel.h"

class QuickSearchLineEdit: public QLineEdit
{
public:
  QuickSearchLineEdit( QuickSearchBar* parent);

  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent *event);

private:
  QuickSearchBar* m_qsBar;
};

QuickSearchLineEdit::QuickSearchLineEdit(QuickSearchBar* parent)
: m_qsBar(parent)
{
  setAcceptDrops(true);
}

void QuickSearchLineEdit::dragEnterEvent(QDragEnterEvent *event)
{
  qDebug() << "QuickSearchLineEdit::dragEnterEvent";
  if (event->mimeData()->hasFormat( TableModelRulesCompiled::ruleMimeType ) ) {
    event->acceptProposedAction();
  }
  else {
    QLineEdit::dragEnterEvent( event );
  }
}

void QuickSearchLineEdit::dropEvent(QDropEvent *event)
{
  qDebug() << "QuickSearchLineEdit::dropEvent";
  if (event->mimeData()->hasFormat(TableModelRulesCompiled::ruleMimeType))
  {
    event->accept();
    event->setDropAction(Qt::CopyAction);
    std::shared_ptr<FilterRuleCompiled> rule(
        new FilterRuleCompiled(event->mimeData()->text()) );

    // Now decide what we can do with this rule...
    auto expression = rule->getExpression();

    bool expressionSet = false;
    auto expressionFind = std::dynamic_pointer_cast<ExpressionFind>( expression );
    auto expressionRe = std::dynamic_pointer_cast<ExpressionRegEx>( expression );

    if (expressionFind) {
      TconstSharedValueGetter vg = expressionFind->getValueGetter();
      std::shared_ptr<const ValueGetterLogEntry> vgLe = std::dynamic_pointer_cast<const ValueGetterLogEntry>(vg);
      if (vgLe && vgLe->getName() == "message") {
        setText(expressionFind->getPattern());
        m_qsBar->setSearchMode(QuickSearchBar::Text);
        expressionSet = true;
      }
    }
    else if (expressionRe) {
      TconstSharedValueGetter vg = expressionRe->getValueGetter();
      std::shared_ptr<const ValueGetterLogEntry> vgLe = std::dynamic_pointer_cast<const ValueGetterLogEntry>(vg);
      if (vgLe && vgLe->getName() == "message") {
        setText(expressionRe->getPattern());
        m_qsBar->setSearchMode(QuickSearchBar::Regex);
        expressionSet = true;
      }
    }

    if (!expressionSet) {
      std::stringstream str;
      str << *expression;
      setText(QString::fromStdString(str.str()));
      m_qsBar->setSearchMode(QuickSearchBar::Expression);
    }
  }
  else
  {
    QLineEdit::dropEvent(event);
  }
}


QuickSearchBar::QuickSearchBar(LogEntryTableWindow* parent
    , std::shared_ptr<LogEntryTableModel> model
    , const QString& colorCode)
: QWidget(parent)
, m_model(model)
, m_logWindow(parent)
, m_searchMode(Text)
{
  // Create quicksearch bar
  m_quickSearch = new QuickSearchLineEdit(this);
  QHBoxLayout* quickSearchLayout = new QHBoxLayout;
  quickSearchLayout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(quickSearchLayout);

  QIcon iconDown, iconUp, search, trash;
  search.addFile(QString::fromUtf8(":/icons/search"), QSize(), QIcon::Normal,
      QIcon::Off);
  iconDown.addFile(QString::fromUtf8(":/icons/searchDown"), QSize(),
      QIcon::Normal, QIcon::Off);
  iconUp.addFile(QString::fromUtf8(":/icons/searchUp"), QSize(), QIcon::Normal,
      QIcon::Off);
  trash.addFile(QString::fromUtf8(":/icons/trash"), QSize(), QIcon::Normal,
      QIcon::Off);

  m_searchModeButton = new QPushButton(tr("Text"));
  QObject::connect(m_searchModeButton, SIGNAL(clicked()), this,
      SLOT(switchSearchMode()));

  m_markButton = new QPushButton(tr("Highlight"));
  m_markButton->setCheckable(true);
  m_markButton->setChecked(true);
  m_markButton->setStyleSheet("QPushButton:checked { background-color: "+ colorCode + "; }");

  QObject::connect(m_markButton, SIGNAL(toggled(bool)), this,
      SLOT(updateSearch()));

  QObject::connect(m_quickSearch, SIGNAL(editingFinished()), this,
      SLOT(updateSearch()));

  QPushButton *searchDnBtn = new QPushButton(iconDown, "");
  QObject::connect(searchDnBtn, SIGNAL(clicked()), this, SLOT(searchNext()));

  QPushButton *searchUpBtn = new QPushButton(iconUp, "");
  QObject::connect(searchUpBtn, SIGNAL(clicked()), this, SLOT(searchPrev()));

  QPushButton *closeBtn = new QPushButton(trash, "");
  QObject::connect(closeBtn, SIGNAL(clicked()), this, SLOT(deleteLater()));

  quickSearchLayout->addWidget(m_searchModeButton);
  quickSearchLayout->addWidget(m_quickSearch);
  quickSearchLayout->addWidget(m_markButton);
  quickSearchLayout->addWidget(searchDnBtn);
  quickSearchLayout->addWidget(searchUpBtn);
  quickSearchLayout->addWidget(closeBtn);

  // Initialize the action for highlighting log entries.
  ActionParser parser(m_model->getParserModelConfiguration());
  if (!parser.parse("rewrite(BG:"+colorCode+")"))
  {
    qDebug() << "Parsing of action failed!";
  }
  else
  {
    m_quickSearchAction = parser.get();
  }

  m_myRuleTableName = m_logWindow->getRuleTable()->addNewUniqueTable();
}

QuickSearchBar::~QuickSearchBar()
{
  m_logWindow->getRuleTable()->clear(m_myRuleTableName);
}


void QuickSearchBar::setSearchMode( SearchModes searchMode )
{
  m_searchMode = searchMode;
  updateSearch();
}

void QuickSearchBar::switchSearchMode()
{
  switch (m_searchMode)
  {
  case Text:
    m_searchMode = Regex;
    break;
  case Regex:
    m_searchMode = Expression;
    break;
  default:
  case Expression:
    m_searchMode = Text;
    break;
  }

  updateSearch();
}

void QuickSearchBar::updateSearch()
{
  switch (m_searchMode)
  {
  default:
  case Text:
    m_searchModeButton->setText(tr("Text"));
    break;
  case Regex:
    m_searchModeButton->setText(tr("Regex"));
    break;
  case Expression:
    m_searchModeButton->setText(tr("Expr"));
    break;
  }

  TSharedRuleTable ruleTableForSearching = m_logWindow->getRuleTable();
  ruleTableForSearching->clear(m_myRuleTableName);
  m_quickSearch->setStyleSheet("");
  m_quickSearch->setToolTip("");
  m_quickSearchExp.reset();

  // Do not do any filtering or searching if no text was given.
  if (m_quickSearch->text().length() == 0)
    return;

  if (m_searchMode == Expression)
  {
    ExpressionParser parser(m_model->getParserModelConfiguration());

    if (parser.parse(m_quickSearch->text()))
    {
      m_quickSearchExp = parser.get();
    }
    else
    {
      m_quickSearch->setStyleSheet("background-color:red;");
      m_quickSearch->setToolTip(
          tr("Error while parsing: ") + parser.getError());
      qDebug() << "Unable to parse expression: " << m_quickSearch->text();
    }
  }
  else if (m_searchMode == Text)
  {
    TSharedValueGetterLogEntry log(
        new ValueGetterLogEntry("message",
            m_model->getParserModelConfiguration()));
    m_quickSearchExp.reset(new ExpressionFind(log, m_quickSearch->text()));

  }
  else if (m_searchMode == Regex)
  {
    TSharedValueGetterLogEntry log(
        new ValueGetterLogEntry("message",
            m_model->getParserModelConfiguration()));
    m_quickSearchExp.reset(new ExpressionRegEx(log, m_quickSearch->text()));
  }

  if (m_markButton->isChecked() && m_quickSearchExp)
  {
    TSharedRule rule(new Rule(m_quickSearchExp, m_quickSearchAction));
    ruleTableForSearching->addRule(m_myRuleTableName, rule);
  }
}


void QuickSearchBar::searchNext()
{
  if (m_quickSearchExp)
    m_logWindow->search(*m_quickSearchExp, false);
}

void QuickSearchBar::searchPrev()
{
  if (m_quickSearchExp)
    m_logWindow->search(*m_quickSearchExp, true);
}
