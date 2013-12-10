#include "CDeck.h"

const CDeck CDeck::INVALID_DECK;

CDeck::CDeck()
: mId(0u)
, mName("")
, mType(EUnknownDeckType)
, mBattlegroundId(0u)
, mCards()
{
}

CDeck::CDeck(TDeckId id, const QString& name, EDeckType type, TBattlegroundId battlegroundId)
: mId(id)
, mName(name)
, mType(type)
, mBattlegroundId(battlegroundId)
, mCards()
{

}

void CDeck::setId(TDeckId id)
{
    mId = id;
}

void CDeck::setName(const QString& name)
{
    mName = name;
}

void CDeck::setType(EDeckType type)
{
    mType = type;
}

void CDeck::setBattleground(TBattlegroundId battlegroundId)
{
    mBattlegroundId = battlegroundId;
}

void CDeck::setCommander(const CCard& card)
{
    if (mCards.empty())
    {
         mCards.push_back(card);
    }
    else if (mCards[0].getType() == ECommanderType)
    {
        mCards[0] = card;
    }
    else
    {
        mCards.push_front(card);
    }
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

void CDeck::trimCards(int maxCards)
{
    if (maxCards < mCards.size())
    {
        TCardList newCards;
        for (int i = 0; i < maxCards; ++i)
        {
            newCards.push_back(mCards[i]);
        }
        mCards = newCards;
    }
}

void CDeck::clearCards()
{
    mCards.clear();
}

TDeckId CDeck::getId() const
{
    return mId;
}

const QString& CDeck::getName() const
{
    return mName;
}

EDeckType CDeck::getType() const
{
    return mType;
}

TBattlegroundId CDeck::getBattlegroundId() const
{
    return mBattlegroundId;
}

const CCard& CDeck::getCommander() const
{
    if (mCards.isEmpty())
    {
        return CCard::INVALID_CARD;
    }
    return mCards.first();
}

const CCard& CDeck::getLegendary() const
{
    for (QList<CCard>::const_iterator i = mCards.begin(); i != mCards.end(); ++i)
    {
        if ((*i).getRarity() == ELegendaryRarity)
        {
            return *i;
        }
    }
    return CCard::INVALID_CARD;
}

const TCardList& CDeck::getCards() const
{
    return mCards;
}

int CDeck::getNumCards() const
{
    return mCards.size();
}

bool CDeck::isValid() const
{
    return !mName.isEmpty() && !mCards.isEmpty();
}

bool CDeck::isEmpty() const
{
    return mCards.isEmpty();
}

