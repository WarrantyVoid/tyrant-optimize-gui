#include "CDeckWidget.h"
#include "ui_DeckWidget.h"
#include "CPathManager.h"

CDeckWidget::CDeckWidget(QWidget *parent)
: QWidget(parent)
, mUi(new Ui::DeckWidget)
, mDecks(CDeckTable::getDeckTable())
, mIsLocked(false)
, mDeck()
{
    mUi->setupUi(this);

    connect(
        mUi->winLabel, SIGNAL(unitDropped()),
        this, SLOT(updateDeck()));
    connect(
        mUi->commanderLabel, SIGNAL(unitDropped()),
        this, SLOT(updateDeck()));
    for(int i = 0; i < 10; ++i)
    {
        QString widgetName = QString("unit%1Label").arg(i, 2, 10, QChar('0'));
        CCardLabel* widget = findChild<CCardLabel*>(widgetName);
        if (widget)
        {
            connect(
                widget, SIGNAL(unitDropped()),
                this, SLOT(updateDeck()));
        }
    }
}

void CDeckWidget::setDropEnabled(bool enabled)
{
    mUi->winLabel->setAcceptDrops(enabled);
    mUi->commanderLabel->setAcceptDrops(enabled);
    for(int i = 0; i < 10; ++i)
    {
        QString widgetName = QString("unit%1Label").arg(i, 2, 10, QChar('0'));
        CCardLabel* widget = findChild<CCardLabel*>(widgetName);
        if (widget)
        {
            widget->setAcceptDrops(enabled);
        }
    }
}

void CDeckWidget::setWinLabel(const QString &text)
{
    if (!mIsLocked)
    {
        mUi->winLabel->setText(text);
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
        setWinLabel("");
        for(int i = -1; i < 10; ++i)
        {
            CCard defaultCard;
            defaultCard.setName(QString("%1").arg(i + 1));
            setUnit(i, defaultCard);
        }
    }
}

void CDeckWidget::setUnit(int slot, const CCard &unit)
{
    if (!mIsLocked)
    {
        CCardLabel* widget = 0;
        if (slot == -1)
        {
            // Commander slot
            widget = mUi->commanderLabel;
        }
        else if (slot > -1 && slot < 10)
        {
            // Assault/Action slot
            QString widgetName = QString("unit%1Label").arg(slot, 2, 10, QChar('0'));
            widget = findChild<CCardLabel*>(widgetName);
        }

        if (widget)
        {
            widget->setCard(unit);
        }
    }
}

void  CDeckWidget::setDeck(const CDeck &deck)
{
    if (!mIsLocked)
    {
        mDeck = deck;
        const QList<CCard> &cards = deck.getCards();
        for (int i = -1; i < 10; ++i)
        {
            if (i + 1 < cards.size())
            {
                setUnit(i, cards.at(i + 1));
            }
            else
            {
               CCard defaultCard;
               defaultCard.setName(QString("%1").arg(i + 1));
               setUnit(i, defaultCard);
            }
        }
    }
}

const CCard& CDeckWidget::getUnit(int slot) const
{
    CCardLabel* widget = 0;
    if (slot == -1)
    {
        // Commander slot
        widget = mUi->commanderLabel;
    }
    else if (slot > -1 && slot < 10)
    {
        // Assault/Action slot
        QString widgetName = QString("unit%1Label").arg(slot, 2, 10, QChar('0'));
        widget = findChild<CCardLabel*>(widgetName);
    }

    if (widget)
    {
        return widget->getCard();
    }
    else
    {
        return CCard::INVALID_CARD;
    }
}

const CDeck& CDeckWidget::getDeck() const
{
    mDeck.clearCards();
    for (int i = -1; i < 10; ++i)
    {
        const CCard &unit = getUnit(i);
        if (unit.isValid())
        {
            mDeck.addCard(unit);
        }
    }
    return mDeck;
}

void CDeckWidget::setDeck(const QString &deckIdParam)
{
    if (!mIsLocked)
    {
        QString deckId = deckIdParam;
        if (!deckId.isEmpty())
        {
            // Choose first deck, if multi decks
            QStringList deckParts = deckId.split(QRegExp("\\;|\\:"), QString::SkipEmptyParts);
            if (deckParts.size() > 1)
            {
                deckId = deckParts.first();
            }

            mDeck = mDecks.getDeckForName(deckId);
            if (mDeck.isValid())
            {
                // Custom Deck
            }
            else if (mDecks.hashToDeck(deckId, mDeck))
            {
                // Deck Hash
            }
            else if (mDecks.strToDeck(deckId, mDeck))
            {
                // Direct Card List
            }
            if (mDeck.isValid())
            {
                setDeck(mDeck);
            }
        }
        else
        {
            setDefaultUnits();
        }
    }
}

void CDeckWidget::updateDeck()
{
    mDeck.clearCards();
    for (int i = -1; i < 10; ++i)
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

