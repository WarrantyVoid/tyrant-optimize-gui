#include "CDeckManagementWidget.h"
#include "ui_DeckManagementWidget.h"
#include <QStandardItemModel>

CDeckManagementWidget::CDeckManagementWidget(QWidget *parent)
: QWidget(parent)
, mUi(new Ui::DeckManagementWidget)
, mDeckIconDelegate()
, mDecks(CDeckTable::getDeckTable())
{
    mUi->setupUi(this);
    mUi->deckTableView->setModel(&mDecks);
    mUi->deckTableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    mUi->deckTableView->horizontalHeader()->setDefaultSectionSize(21);
    mUi->deckTableView->verticalHeader()->setDefaultSectionSize(18);
    mUi->deckTableView->setItemDelegateForColumn(1, &mDeckIconDelegate);
    connect(
        mUi->deckTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this, SLOT(updateButtonAvailability()));
    connect(
        mUi->deckTableView, SIGNAL(doubleClicked(const QModelIndex &)),
        this, SLOT(setSelectedDecks()));

    connect(
        mUi->deleteDeckButton, SIGNAL(clicked()),
        this, SLOT(deleteSelectedDecks()));
    connect(
        mUi->setEnemyDeckButton, SIGNAL(clicked()),
        this, SLOT(setSelectedEnemyDecks()));
    connect(
        mUi->setBaseDeckButton, SIGNAL(clicked()),
        this, SLOT(setSelectedBaseDeck()));
}

CDeckManagementWidget::~CDeckManagementWidget()
{
    delete mUi;
    mUi = 0;
}

void CDeckManagementWidget::updateButtonAvailability()
{
    bool deleteAllowed(false);
    bool setBaseDeckAllowed(false);
    bool setEnemyDeckAllowed(false);

    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    if (!indexes.isEmpty())
    {
        deleteAllowed = true;
        setBaseDeckAllowed = true;
        setEnemyDeckAllowed = true;
        int deckSelectCount = 0;
        for (QModelIndexList::const_iterator i = indexes.begin(); i!= indexes.end(); ++i)
        {
            const CDeck &deck = mDecks.getDeckForIndex(*i);
            ++deckSelectCount;
            if (deleteAllowed && deck.getType() != ECustomDeckType)
            {
                deleteAllowed = false;
            }
            if (setBaseDeckAllowed && deck.getType() != ECustomDeckType)
            {
                setBaseDeckAllowed = false;
            }
        }
        if (setBaseDeckAllowed && deckSelectCount > 1)
        {
            setBaseDeckAllowed = false;
        }
    }
    mUi->deleteDeckButton->setEnabled(deleteAllowed);
    mUi->setBaseDeckButton->setEnabled(setBaseDeckAllowed);
    mUi->setEnemyDeckButton->setEnabled(setEnemyDeckAllowed);
}

void CDeckManagementWidget::deleteSelectedDecks()
{
    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    QStringList selectedDecks;
    if (!indexes.isEmpty())
    {
        for (QModelIndexList::const_iterator i = indexes.begin(); i!= indexes.end(); ++i)
        {
            const CDeck &deck = mDecks.getDeckForIndex(*i);
            selectedDecks.push_back(deck.getName());
        }
        mDecks.deleteCustomDecks(selectedDecks);
    }
}

void CDeckManagementWidget::setSelectedEnemyDecks()
{
    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    QStringList selectedDecks;
    if (!indexes.isEmpty())
    {
        for (QModelIndexList::const_iterator i = indexes.begin(); i!= indexes.end(); ++i)
        {
            const CDeck &deck = mDecks.getDeckForIndex(*i);
            selectedDecks.push_back(deck.getName());
        }
        emit setEnemyDeck(selectedDecks.join(";"));
    }
}

void CDeckManagementWidget::setSelectedBaseDeck()
{
    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    if (!indexes.isEmpty())
    {
        const CDeck &deck = mDecks.getDeckForIndex(indexes.first());
        emit setBaseDeck(deck.getName());
    }
}

void CDeckManagementWidget::setSelectedDecks()
{
    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    QStringList selectedDecks;
    if (!indexes.isEmpty())
    {
        for (QModelIndexList::const_iterator i = indexes.begin(); i!= indexes.end(); ++i)
        {
            const CDeck &deck = mDecks.getDeckForIndex(*i);
            selectedDecks.push_back(deck.getName());
        }
        if (mUi->setBaseDeckButton->isEnabled())
        {
            if (mUi->setEnemyDeckButton->isEnabled())
            {
                emit setBaseOrEnemyDeck(selectedDecks.first());
            }
            else
            {
                emit setBaseDeck(selectedDecks.first());
            }
        }
        else
        {
            emit setEnemyDeck(selectedDecks.join(";"));
        }
    }
}