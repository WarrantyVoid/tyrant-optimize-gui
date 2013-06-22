#include "CDeckManagementWidget.h"
#include "ui_DeckManagementWidget.h"
#include <QStandardItemModel>

CDeckManagementWidget::CDeckManagementWidget(QWidget *parent)
: QWidget(parent)
, mUi(new Ui::DeckManagementWidget)
, mDecks(CDeckTable::getDeckTable())
, mDeckIconDelegate()
, mDeckSortProxy()
{
    mUi->setupUi(this);
    mDeckSortProxy.setSourceModel(&mDecks);
    mUi->deckTableView->setModel(&mDeckSortProxy);
    mUi->deckTableView->setSortingEnabled(true);
    mUi->deckTableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    mUi->deckTableView->horizontalHeader()->setDefaultSectionSize(21);
    mUi->deckTableView->verticalHeader()->setDefaultSectionSize(18);
    mUi->deckTableView->setItemDelegateForColumn(1, &mDeckIconDelegate);
    connect(
        mUi->deckTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this, SLOT(updateButtonAvailability()));
    connect(
        mUi->deckTableView, SIGNAL(doubleClicked(const QModelIndex &)),
        this, SLOT(setSelectedDeck()));
    connect(
        mUi->deckTableView->horizontalHeader(), SIGNAL(sectionClicked(int)),
        this, SLOT(sortDeckTableRows(int)));

    connect(
        mUi->deleteDeckButton, SIGNAL(clicked()),
        this, SLOT(deleteSelectedDeck()));
    connect(
        mUi->setEnemyDeckButton, SIGNAL(clicked()),
        this, SLOT(setSelectedEnemyDeck()));
    connect(
        mUi->setBaseDeckButton, SIGNAL(clicked()),
        this, SLOT(setSelectedBaseDeck()));

    connect(
        mUi->nameBox, SIGNAL(editTextChanged(const QString&)),
        this, SLOT(updateView()));
    connect(
        mUi->commanderBox, SIGNAL(editTextChanged(const QString&)),
        this, SLOT(updateView()));
    connect(
        mUi->customBox, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->missionBox, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->questBox, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->raidBox, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
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
            const CDeck &deck = mDecks.getDeckForIndex(mDeckSortProxy.mapToSource(*i));
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

void CDeckManagementWidget::updateView()
{
    QStringList deckTypes;
    if (mUi->customBox->isChecked())
    {
        deckTypes << "C";
    }
    if (mUi->missionBox->isChecked())
    {
        deckTypes << "M";
    }
    if (mUi->questBox->isChecked())
    {
        deckTypes << "Q";
    }
    if (mUi->raidBox->isChecked())
    {
        deckTypes << "R";
    }
    mDeckSortProxy.setFilterPattern(0, deckTypes.join("|"));
    mDeckSortProxy.setFilterPattern(1, mUi->commanderBox->currentText());
    mDeckSortProxy.setFilterPattern(3, mUi->nameBox->currentText());
    mDeckSortProxy.invalidate();
}

void CDeckManagementWidget::deleteSelectedDeck()
{
    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    QStringList selectedDecks;
    if (!indexes.isEmpty())
    {
        for (QModelIndexList::const_iterator i = indexes.begin(); i!= indexes.end(); ++i)
        {
            const CDeck &deck = mDecks.getDeckForIndex(mDeckSortProxy.mapToSource(*i));
            selectedDecks.push_back(deck.getName());
        }
        mDecks.deleteCustomDecks(selectedDecks);
    }
}

void CDeckManagementWidget::setSelectedBaseDeck()
{
    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    QStringList selectedDecks;
    if (!indexes.isEmpty())
    {
        for (QModelIndexList::const_iterator i = indexes.begin(); i!= indexes.end(); ++i)
        {
            const CDeck &deck = mDecks.getDeckForIndex(mDeckSortProxy.mapToSource(*i));
            selectedDecks.push_back(deck.getName());
        }
        emit setDeck(selectedDecks.join(";"), BaseDeckInputTarget);
    }
}

void CDeckManagementWidget::setSelectedEnemyDeck()
{
    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    QStringList selectedDecks;
    if (!indexes.isEmpty())
    {
        for (QModelIndexList::const_iterator i = indexes.begin(); i!= indexes.end(); ++i)
        {
            const CDeck &deck = mDecks.getDeckForIndex(mDeckSortProxy.mapToSource(*i));
            selectedDecks.push_back(deck.getName());
        }
        emit setDeck(selectedDecks.join(";"), EnemyDeckInputTarget);
    }
}

void CDeckManagementWidget::setSelectedDeck()
{
    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    QStringList selectedDecks;
    if (!indexes.isEmpty())
    {
        for (QModelIndexList::const_iterator i = indexes.begin(); i!= indexes.end(); ++i)
        {
            const CDeck &deck = mDecks.getDeckForIndex(mDeckSortProxy.mapToSource(*i));
            selectedDecks.push_back(deck.getName());
        }
        if (mUi->setBaseDeckButton->isEnabled())
        {
            if (mUi->setEnemyDeckButton->isEnabled())
            {
                emit setDeck(selectedDecks.join(";"), ActiveDeckInputTarget);
            }
            else
            {
                emit setDeck(selectedDecks.join(";"), BaseDeckInputTarget);
            }
        }
        else
        {
            emit setDeck(selectedDecks.join(";"), EnemyDeckInputTarget);
        }
    }
}

void CDeckManagementWidget::sortDeckTableRows(int column)
{
    mUi->deckTableView->sortByColumn(column);
}
