#include "CDeckManagementWidget.h"
#include "ui_DeckManagementWidget.h"
#include "CVersion.h"
#include <QStandardItemModel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QMenu>
#include <QClipboard>

CDeckManagementWidget::CDeckManagementWidget(QWidget *parent)
: QWidget(parent)
, mUi(new Ui::DeckManagementWidget)
, mDecks(CDeckTable::getDeckTable())
, mDeckItemDelegate()
, mDeckSortProxy()
{
    mUi->setupUi(this);
    mDeckSortProxy.setSourceModel(&mDecks);
    mUi->deckTableView->setModel(&mDeckSortProxy);
    mUi->deckTableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    mUi->deckTableView->horizontalHeader()->setDefaultSectionSize(21);
    mUi->deckTableView->verticalHeader()->setDefaultSectionSize(18);
    mUi->deckTableView->setItemDelegate(&mDeckItemDelegate);
    mUi->deckTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mUi->deckTableView->installEventFilter(this);
    mUi->deckTableView->viewport()->installEventFilter(this);

    // Table
    connect(
        mUi->deckTableView, SIGNAL(customContextMenuRequested(QPoint)),
        this, SLOT(displayContextMenu(QPoint)));
    connect(
        mUi->deckTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this, SLOT(updateButtonAvailability()));

    // Search fields
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

    // Menu
    connect(
        mUi->setEnemyDeckAction, SIGNAL(triggered()),
        this, SLOT(setSelectedEnemyDeck()));
    connect(
        mUi->setBaseDeckAction, SIGNAL(triggered()),
        this, SLOT(setSelectedBaseDeck()));
    connect(
        mUi->nameDeckAction, SIGNAL(triggered()),
        this, SLOT(nameSelectedDeck()));
    connect(
        mUi->hashDeckAction, SIGNAL(triggered()),
        this, SLOT(hashSelectedDeck()));
    connect(
        mUi->blockDeckAction, SIGNAL(triggered()),
        this, SLOT(blockSelectedDeck()));
    connect(
        mUi->deleteDeckAction, SIGNAL(triggered()),
        this, SLOT(deleteSelectedDeck()));
}

CDeckManagementWidget::~CDeckManagementWidget()
{
    delete mUi;
    mUi = 0;
}

void CDeckManagementWidget::loadParameterSettings(QSettings &settings)
{
    settings.beginGroup("deckManagement");
    CVersion cfgVersion = CVersion(settings.value("general/version").toString());
    if (cfgVersion.isValid())
    {
        if (cfgVersion > CVersion(1,4,5))
        {
            mUi->deckTableView->horizontalHeader()->restoreState(settings.value("tableHeader").toByteArray());
        }
    }
    mUi->nameBox->lineEdit()->setText(settings.value("nameFilter", "").toString());
    mUi->commanderBox->lineEdit()->setText(settings.value("commanderFilter", "").toString());
    mUi->customBox->setChecked(settings.value("customDeckFilter", true).toBool());
    mUi->missionBox->setChecked(settings.value("missionDeckFilter", false).toBool());
    mUi->questBox->setChecked(settings.value("questDeckFilter", false).toBool());
    mUi->raidBox->setChecked(settings.value("raidDeckFilter", false).toBool());

    QStringList blockList = settings.value("blackListedDecks").toStringList();
    for (QStringList::const_iterator i = blockList.begin(); i != blockList.end(); ++i)
    {
        mDecks.setDeckBlockage(*i, true);
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

void CDeckManagementWidget::displayContextMenu(const QPoint &/*pos*/)
{
    QMenu menu ;
    menu.addAction(mUi->setBaseDeckAction);
    menu.addAction(mUi->setEnemyDeckAction);
    menu.addAction(mUi->deleteDeckAction);
    menu.addSeparator();
    menu.addAction(mUi->nameDeckAction);
    menu.addAction(mUi->hashDeckAction);
    menu.addSeparator();
    menu.addAction(mUi->blockDeckAction);
    menu.exec(QCursor::pos());
}

bool CDeckManagementWidget::addCustomDeck(CDeck &customDeck)
{
    bool result = mDecks.addCustomDeck(customDeck);
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
        mUi->blockDeckAction->setChecked(mDecks.isDeckBlocked(deck.getName()));
    }
    mUi->deleteDeckAction->setEnabled(deleteAllowed);
    mUi->blockDeckAction->setEnabled(blockAllowed);
    mUi->setBaseDeckAction->setEnabled(setBaseDeckAllowed);
    mUi->setEnemyDeckAction->setEnabled(setEnemyDeckAllowed);
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
    mDeckSortProxy.setFilterPattern(EDeckTypeColumn, deckTypes.join("|"));
    mDeckSortProxy.setFilterPattern(EDeckCommanderColumn, mUi->commanderBox->currentText());
    mDeckSortProxy.setFilterPattern(EDeckNameColumn, mUi->nameBox->currentText());
    mDeckSortProxy.invalidate();

    QString finderResult = QString("Displaying %1 of %2 available decks")
        .arg(mUi->deckTableView->model()->rowCount())
        .arg(mDecks.getNumberOfDecks());
    mUi->finderLabel->setText(finderResult);
}

void CDeckManagementWidget::deleteSelectedDeck()
{
    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    if (!indexes.isEmpty())
    {
        QStringList selectedDecks;
        for (QModelIndexList::const_iterator i = indexes.begin(); i!= indexes.end(); ++i)
        {
            const CDeck &deck = mDecks.getDeckForIndex(mDeckSortProxy.mapToSource(*i));
            selectedDecks.push_back(deck.getName());
        }
        mDecks.deleteCustomDecks(selectedDecks);
    }
}

void CDeckManagementWidget::nameSelectedDeck()
{
    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    if (!indexes.isEmpty())
    {
        const CDeck &deck = mDecks.getDeckForIndex(mDeckSortProxy.mapToSource(indexes.first()));
        QString str;
        if (mDecks.deckToStr(deck, str))
        {
            QApplication::clipboard()->setText(str);
        }
    }
}

void CDeckManagementWidget::hashSelectedDeck()
{
    QModelIndexList indexes = mUi->deckTableView->selectionModel()->selectedRows();
    if (!indexes.isEmpty())
    {
        const CDeck &deck = mDecks.getDeckForIndex(mDeckSortProxy.mapToSource(indexes.first()));
        QString hash;
        if (mDecks.deckToHash(deck, hash))
        {
            QApplication::clipboard()->setText(hash);
        }
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
        if (mUi->setBaseDeckAction->isEnabled())
        {
            if (mUi->setEnemyDeckAction->isEnabled())
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
        case QEvent::KeyRelease:
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);
            if (!mDeckItemDelegate.isEditing())
            {
                switch (keyEvent->key())
                {
                    case Qt::Key_Delete:
                        deleteSelectedDeck();
                        return true;
                    default:
                        break;
                }
            }
        }
        default:
        {
            break;
        }
    }
    return QObject::eventFilter(obj, e);
}
