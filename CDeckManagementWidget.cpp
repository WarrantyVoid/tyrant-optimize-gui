#include "CDeckManagementWidget.h"
#include "ui_DeckManagementWidget.h"
#include <QStandardItemModel>
#include <QLineEdit>
#include <QMouseEvent>

CDeckManagementWidget::CDeckManagementWidget(QWidget *parent)
: QWidget(parent)
, mUi(new Ui::DeckManagementWidget)
, mDecks(CDeckTable::getDeckTable())
, mDeckItemDelegate()
, mDeckSortProxy()
{
    mUi->setupUi(this);
    toggleDeckUsageButton(false);
    mDeckSortProxy.setSourceModel(&mDecks);
    mUi->deckTableView->setModel(&mDeckSortProxy);
    mUi->deckTableView->setSortingEnabled(true);
    mUi->deckTableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    mUi->deckTableView->horizontalHeader()->setDefaultSectionSize(21);
    mUi->deckTableView->verticalHeader()->setDefaultSectionSize(18);
    mUi->deckTableView->setItemDelegate(&mDeckItemDelegate);
    mUi->deckTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mUi->deckTableView->viewport()->installEventFilter(this);

    connect(
        mUi->deckTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this, SLOT(updateButtonAvailability()));
    connect(
        mUi->deleteDeckButton, SIGNAL(clicked()),
        this, SLOT(deleteSelectedDeck()));
    connect(
        mUi->blockDeckButton, SIGNAL(clicked()),
        this, SLOT(blockSelectedDeck()));
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
    mUi->deckTableView->horizontalHeader()->restoreState(settings.value("tableHeader").toByteArray());

    QStringList blockList = settings.value("blackListedDecks").toStringList();
    for (QStringList::const_iterator i = blockList.begin(); i != blockList.end(); ++i)
    {
        mDecks.setDeckBlockage(*i, true);
        const CDeck& deck = mDecks.getDeckForName(*i);
        if (deck.isValid())
        {
            emit deckBlockageChanged(deck, true);
        }
    }
    settings.endGroup();

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
    settings.setValue("tableHeader", mUi->deckTableView->horizontalHeader()->saveState());
    settings.setValue("sortColumn", "0");
    settings.setValue("sortOrder", "0");

    QStringList blockList;
    QStringList deckList;
    mDecks.getCustomDecks(deckList);
    for (QStringList::const_iterator i = deckList.begin(); i != deckList.end(); ++i)
    {
        if (mDecks.isDeckBlocked(*i))
        {
            blockList.push_back(*i);
        }
    }
    settings.setValue("blackListedDecks", blockList);
    settings.endGroup();
}

bool CDeckManagementWidget::addCustomDeck(CDeck &customDeck)
{
    bool isBlocked = mDecks.isDeckBlocked(customDeck.getName());
    if (isBlocked)
    {
        const CDeck& exDeck = mDecks.getDeckForName(customDeck.getName());
        if (exDeck.isValid())
        {
            emit deckBlockageChanged(exDeck, false);
        }
    }
    bool result = mDecks.addCustomDeck(customDeck);
    if (isBlocked)
    {
        emit deckBlockageChanged(customDeck, true);
    }
    mDeckSortProxy.invalidate();
    return result;
}

void CDeckManagementWidget::updateButtonAvailability()
{
    bool deleteAllowed(false);
    bool blockAllowed(false);
    bool setBaseDeckAllowed(false);
    bool setEnemyDeckAllowed(false);
    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    if (!indexes.isEmpty())
    {
        deleteAllowed = true;
        blockAllowed = true;
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
            if (blockAllowed && deck.getType() != ECustomDeckType)
            {
                blockAllowed = false;
            }
            if (setBaseDeckAllowed && deck.getType() != ECustomDeckType)
            {
                setBaseDeckAllowed = false;
            }
        }
        if (blockAllowed && deckSelectCount > 1)
        {
            blockAllowed = false;
        }
        if (setBaseDeckAllowed && deckSelectCount > 1)
        {
            setBaseDeckAllowed = false;
        }
        const CDeck &deck = mDecks.getDeckForIndex(mDeckSortProxy.mapToSource(indexes.first()));
        toggleDeckUsageButton(mDecks.isDeckBlocked(deck.getName()));
    }
    mUi->deleteDeckButton->setEnabled(deleteAllowed);
    mUi->blockDeckButton->setEnabled(blockAllowed);
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
    mDeckSortProxy.setFilterPattern(1, deckTypes.join("|"));
    mDeckSortProxy.setFilterPattern(2, mUi->commanderBox->currentText());
    mDeckSortProxy.setFilterPattern(4, mUi->nameBox->currentText());
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
            if (mDecks.isDeckBlocked(deck.getName()))
            {
                // unblock deleted decks automatically
                emit deckBlockageChanged(deck, false);
            }
        }
        mDecks.deleteCustomDecks(selectedDecks);
    }
}

void CDeckManagementWidget::blockSelectedDeck()
{
    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    if (!indexes.isEmpty())
    {
        const CDeck &deck = mDecks.getDeckForIndex(mDeckSortProxy.mapToSource(indexes.first()));
        bool isBlocked = !mDecks.isDeckBlocked(deck.getName());
        mDecks.setDeckBlockage(deck.getName(), isBlocked);
        emit deckBlockageChanged(deck, isBlocked);
        mDeckSortProxy.invalidate();
        toggleDeckUsageButton(isBlocked);
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

bool CDeckManagementWidget::eventFilter(QObject *obj, QEvent *e)
{
    switch(e->type())
    {
        case QEvent::MouseButtonDblClick:
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                setSelectedDeck();
                return true;
            }
            return QObject::eventFilter(obj, e);

        }
        case QEvent::MouseButtonRelease:
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
            if (mouseEvent->button() == Qt::RightButton
                && mUi->blockDeckButton->isEnabled())
            {
                blockSelectedDeck();
                return true;
            }
            return QObject::eventFilter(obj, e);
        }
        default:
        {
            // standard event processing
            return QObject::eventFilter(obj, e);
        }
    }
}

void CDeckManagementWidget::toggleDeckUsageButton(bool curDeckUsed)
{
    if (curDeckUsed)
    {
        mUi->blockDeckButton->setText("Unblock");
    }
    else
    {
       mUi->blockDeckButton->setText("Block");
    }
}
