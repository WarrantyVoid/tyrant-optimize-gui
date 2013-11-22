#include "CDeckWidget.h"
#include "ui_DeckWidget.h"
#include "CGlobalConfig.h"

CDeckWidget::CDeckWidget(QWidget *parent)
: QWidget(parent)
, mUi(new Ui::DeckWidget)
, mCardLabels()
, mDecks(CDeckTable::getDeckTable())
, mIsLocked(false)
, mDeck()
, mMaxCards(20)
{
    mUi->setupUi(this);

    // Init card labels for indexed widget acess
    mCardLabels.push_back(mUi->commanderLabel);
    for(int iSlot = 0; iSlot < 20; ++iSlot)
    {
        QString widgetName = QString("unit%1Label").arg(iSlot, 2, 10, QChar('0'));
        mCardLabels.push_back(findChild<CCardLabel*>(widgetName));
    }
    syncScrollArea(10);

    // Initialize widget connections
    connect(
        mUi->winLabel, SIGNAL(trashTriggered()),
            this, SLOT(clearDeck()));
    connect(
        mUi->winLabel, SIGNAL(unitDropped()),
        this, SLOT(updateDeck()));
    for(int iSlot = -1; iSlot < mMaxCards; ++iSlot)
    {
        CCardLabel *widget = getLabelForSlot(iSlot);
        if (widget)
        {
            connect(
                widget, SIGNAL(unitDropped()),
                this, SLOT(updateDeck()));
            connect(widget, SIGNAL(unitDoubleClicked()),
                this, SLOT(updateDeck()));
        }
    }

}

void CDeckWidget::setDropEnabled(bool enabled)
{
    mUi->winLabel->setAcceptDrops(enabled);
    for(int i = -1; i < mMaxCards; ++i)
    {
        CCardLabel *widget = getLabelForSlot(i);
        if (widget)
        {
            widget->setAcceptDrops(enabled);
        }
    }
}

void CDeckWidget::setLockEnabled(bool enabled)
{
    for(int i = -1; i < mMaxCards; ++i)
    {
        CCardLabel *widget = getLabelForSlot(i);
        if (widget)
        {
            widget->setLockEnabled(enabled);
        }
    }
}

void CDeckWidget::setLocked(int slot, bool locked)
{
    CCardLabel* widget = getLabelForSlot(slot);
    if (widget)
    {
        widget->setLocked(locked);
    }
}

bool CDeckWidget::isLocked(int slot) const
{
    CCardLabel* widget = getLabelForSlot(slot);
    if (widget)
    {
        return widget->isLocked();
    }
    return false;
}

void CDeckWidget::setMaxCards(int maxCards)
{
    mMaxCards = maxCards;
    mDeck.trimCards(mMaxCards + 1);
    syncAllUnits();
}

int CDeckWidget::getMaxCards() const
{
    return mMaxCards;
}

void CDeckWidget::setWinLabel(const SOptimizationStatus &status, EOptimizationMode mode)
{
    if (!mIsLocked)
    {
        mUi->winLabel->setWinText(status, mode);
    }
}

void CDeckWidget::setWinLabel(const QPixmap &pixmap)
{
    if (!mIsLocked)
    {
        mUi->winLabel->setPixmap(pixmap);
    }
}

void CDeckWidget::setDefaultUnits()
{
    if (!mIsLocked)
    {
        mDeck.clearCards();
        syncAllUnits();
    }
}

void  CDeckWidget::setDeck(const CDeck &deck)
{
    if (!mIsLocked)
    {
        mDeck = deck;
        syncAllUnits();
    }
}

const CDeck& CDeckWidget::getDeck() const
{
    return mDeck;
}

void CDeckWidget::setDeck(const QString &deckSpecParam, EDeckSpecificationType specType)
{
    if (!mIsLocked)
    {
        QString deckSpec = deckSpecParam;
        if (!deckSpec.isEmpty())
        {
            // Choose first deck, if multi decks
            QStringList deckParts = deckSpec.split(QRegExp("\\;|\\:"), QString::SkipEmptyParts);
            if (deckParts.size() > 1)
            {
                deckSpec = deckParts.first();
            }

            switch(specType)
            {
            case ENameSpecification:
                mDeck = mDecks.getDeckForName(deckSpec);
                break;
            case EHashSpecification:
                mDecks.hashToDeck(deckSpec, mDeck);
                break;
            case ECardListSpecification:
                mDecks.strToDeck(deckSpec, mDeck);
                break;
            default:
                mDeck = mDecks.getDeckForName(deckSpec);
                if (!mDeck.isValid())
                {
                    if (!mDecks.hashToDeck(deckSpec, mDeck))
                    {
                        mDecks.strToDeck(deckSpec, mDeck);
                    }
                }
            }
            syncAllUnits();
        }
        else
        {
            setDefaultUnits();
        }
    }
}

void CDeckWidget::clearDeck()
{
    setDefaultUnits();
    emit deckChanged("");
}

void CDeckWidget::updateView()
{
    mUi->deckScrollAreaWidgetContents->update();
    update();
}

void CDeckWidget::updateDeck()
{
    mDeck.clearCards();
    for (int i = -1; i < mMaxCards; ++i)
    {
        const CCard &unit = getUnit(i);
        if (unit.isValid())
        {
            mDeck.addCard(unit);
        }
    }
    QString deckId;
    if (mDecks.deckToHash(mDeck, deckId))
    {
        mIsLocked = true;
        emit deckChanged(deckId);
        mIsLocked = false;
    }
}

const CCard& CDeckWidget::getUnit(int slot) const
{
    CCardLabel* widget = getLabelForSlot(slot);
    if (widget)
    {
        return widget->getCard();
    }
    else
    {
        return CCard::INVALID_CARD;
    }
}

void CDeckWidget::setUnit(int slot, const CCard &unit)
{
    if (!mIsLocked)
    {
        CCardLabel* widget = getLabelForSlot(slot);
        if (widget)
        {
            widget->setCard(unit);
        }
    }
}

void CDeckWidget::syncAllUnits()
{
    const QList<CCard> &cards = mDeck.getCards();
    const int maxCards = qMin(cards.size(), mMaxCards + 1);
    syncScrollArea(maxCards - 1);

    // Sync lock state
    bool newLockState[maxCards];
    for (int iLock = 0; iLock < maxCards; ++iLock)
    {
        newLockState[iLock] = false;
    }
    for (int iOld = 0; iOld < maxCards; ++iOld)
    {
        if (mCardLabels[iOld] && mCardLabels[iOld]->isLocked())
        {
            if (mCardLabels[iOld]->getCard().isValid()
                && (mCardLabels[iOld]->getCard().getId() != cards[iOld].getId() || newLockState[iOld]))
            {
                for (int iNew = 0; iNew < maxCards; ++iNew)
                {
                    if (mCardLabels[iNew]
                        && mCardLabels[iOld]->getCard().getId() == cards[iNew].getId()
                        && newLockState[iNew] == false)
                    {
                        newLockState[iNew] = true;
                        break;
                    }
                }
            }
            else
            {
                newLockState[iOld] = true;
            }
        }
    }

    for (int iCard = 0; iCard < mCardLabels.size(); ++iCard)
    {
        if (iCard < cards.size())
        {
            setLocked(iCard - 1, newLockState[iCard]);
            setUnit(iCard - 1, cards.at(iCard));
        }
        else
        {
           setLocked(iCard - 1, false);
           setUnit(iCard - 1, CCard::INVALID_CARD);
        }
    }
}

void CDeckWidget::syncScrollArea(int numCards)
{
    for(int iLabel = 11; iLabel < mCardLabels.size(); ++iLabel)
    {
        if (mCardLabels[iLabel])
        {
            mCardLabels[iLabel]->setVisible(iLabel <= numCards);
        }
    }
    mUi->deckScrollAreaWidgetContents->setGeometry(0, 0, 420, qMax(220, (numCards + 4)  / 5 * 110));

}

CCardLabel* CDeckWidget::getLabelForSlot(int slot) const
{
    if (slot > -2 && slot < mMaxCards)
    {
        return mCardLabels[slot + 1];
    }
    return 0;
}

