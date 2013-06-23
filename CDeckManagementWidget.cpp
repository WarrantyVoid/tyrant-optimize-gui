#include "CDeckManagementWidget.h"
#include "ui_DeckManagementWidget.h"
#include <QStandardItemModel>
#include <QLineEdit>

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
        mUi->deleteDeckButton, SIGNAL(clicked()),
        this, SLOT(deleteSelectedDeck()));
    connect(
        mUi->blacklistDeckButton, SIGNAL(clicked()),
        this, SLOT(blacklistSelectedDeck()));
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

void CDeckManagementWidget::loadParameterSettings(QSettings &settings)
{
    settings.beginGroup("deckManagement");
    mUi->nameBox->lineEdit()->setText(settings.value("nameFilter", "").toString());
    mUi->commanderBox->lineEdit()->setText(settings.value("commanderFilter", "").toString());
    mUi->customBox->setChecked(settings.value("customDeckFilter", true).toBool());
    mUi->missionBox->setChecked(settings.value("missionDeckFilter", false).toBool());
    mUi->questBox->setChecked(settings.value("questDeckFilter", false).toBool());
    mUi->raidBox->setChecked(settings.value("raidDeckFilter", false).toBool());

    QStringList blackList = settings.value("blackListedDecks").toStringList();
    for (QStringList::const_iterator i = blackList.begin(); i != blackList.end(); ++i)
    {
        mDecks.setDeckBlackListed(*i, true);
    }
    settings.endGroup();

    mDeckSortProxy.sort(0, Qt::AscendingOrder);
    updateView();
}

void CDeckManagementWidget::saveParameterSettings(QSettings &settings)
{
    settings.beginGroup("deckManagement");
    settings.setValue("nameFilter", mUi->nameBox->currentText());
    settings.setValue("commanderFilter", mUi->commanderBox->currentText());
    settings.setValue("customDeckFilter", mUi->customBox->isChecked());
    settings.setValue("missionDeckFilter", mUi->missionBox->isChecked());
    settings.setValue("questDeckFilter", mUi->questBox->isChecked());
    settings.setValue("raidDeckFilter", mUi->raidBox->isChecked());

    QStringList blackList;
    QStringList deckList;
    mDecks.getCustomDecks(deckList);
    for (QStringList::const_iterator i = deckList.begin(); i != deckList.end(); ++i)
    {
        if (mDecks.isDeckBlackListed(*i))
        {
            blackList.push_back(*i);
        }
    }
    settings.setValue("blackListedDecks", blackList);
    settings.endGroup();
}

bool CDeckManagementWidget::addCustomDeck(CDeck &customDeck)
{
    bool isBlack = mDecks.isDeckBlackListed(customDeck.getName());
    if (isBlack)
    {
        const CDeck& exDeck = mDecks.getDeckForName(customDeck.getName());
        if (exDeck.isValid())
        {
            QStringList blackList;
            addDeckToBlackList(exDeck, blackList);
            emit blackListCards(blackList, false);
        }
    }
    bool result = mDecks.addCustomDeck(customDeck);
    if (isBlack)
    {
        QStringList blackList;
        addDeckToBlackList(customDeck, blackList);
        emit blackListCards(blackList, true);
    }
    return result;
}

void CDeckManagementWidget::updateButtonAvailability()
{
    bool deleteAllowed(false);
    bool blackListAllowed(false);
    bool setBaseDeckAllowed(false);
    bool setEnemyDeckAllowed(false);

    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    if (!indexes.isEmpty())
    {
        deleteAllowed = true;
        blackListAllowed = true;
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
            if (blackListAllowed && deck.getType() != ECustomDeckType)
            {
                blackListAllowed = false;
            }
            if (setBaseDeckAllowed && deck.getType() != ECustomDeckType)
            {
                setBaseDeckAllowed = false;
            }
        }
        if (blackListAllowed && deckSelectCount > 1)
        {
            blackListAllowed = false;
        }
        if (setBaseDeckAllowed && deckSelectCount > 1)
        {
            setBaseDeckAllowed = false;
        }
    }
    mUi->deleteDeckButton->setEnabled(deleteAllowed);
    mUi->blacklistDeckButton->setEnabled(blackListAllowed);
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
        QStringList blackList;
        for (QModelIndexList::const_iterator i = indexes.begin(); i!= indexes.end(); ++i)
        {
            const CDeck &deck = mDecks.getDeckForIndex(mDeckSortProxy.mapToSource(*i));
            selectedDecks.push_back(deck.getName());
            if (mDecks.isDeckBlackListed(deck.getName()))
            {
                addDeckToBlackList(deck, blackList);
            }
        }
        // un-blacklist deleted decks automatically
        emit blackListCards(blackList, false);
        mDecks.deleteCustomDecks(selectedDecks);
    }
}

void CDeckManagementWidget::blacklistSelectedDeck()
{
    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    if (!indexes.isEmpty())
    {
        QStringList blackList;
        const CDeck &deck = mDecks.getDeckForIndex(mDeckSortProxy.mapToSource(indexes.first()));
        bool isBlack = !mDecks.isDeckBlackListed(deck.getName());
        mDecks.setDeckBlackListed(deck.getName(), isBlack);
        addDeckToBlackList(deck, blackList);
        emit blackListCards(blackList, isBlack);
        mDeckSortProxy.invalidate();
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

void CDeckManagementWidget::addDeckToBlackList(const CDeck& deck, QStringList &blackList)
{
    const QList<CCard> &cards = deck.getCards();
    for (int i = 0; i < cards.size(); ++i)
    {
        blackList << cards[i].getName();
    }
}
