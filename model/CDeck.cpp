#include "CDeck.h"

const CDeck CDeck::INVALID_DECK;

CDeck::CDeck()
: mName("")
, mType(EUnknownDeckType)
, mCards()
{
}

CDeck::CDeck(const QString& name, EDeckType type)
: mName(name)
, mType(type)
, mCards()
{

}

void CDeck::setName(const QString& name)
{
    mName = name;
}

void CDeck::setType(EDeckType type)
{
    mType = type;
}

void CDeck::addCard(const CCard& card)
{
    mCards.push_back(card);
}

void CDeck::replaceCard(int index, const CCard& card)
{
    if (index <  mCards.size())
    {
        mCards[index] = card;
    }
}

void CDeck::clearCards()
{
    mCards.clear();
}

const QString& CDeck::getName() const
{
    return mName;
}

EDeckType CDeck::getType() const
{
    return mType;
}

const QList<CCard>& CDeck::getCards() const
{
    return mCards;
}

int CDeck::getNumCards() const
{
    return mCards.size();
}

bool CDeck::isValid() const
{
    return !mName.isEmpty() && !mCards.isEmpty() && mCards.first().getType() == ECommanderType;
}

bool CDeck::isEmpty() const
{
    return mCards.isEmpty();
}

