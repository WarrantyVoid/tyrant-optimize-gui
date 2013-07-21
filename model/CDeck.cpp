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

CDeck::CDeck(unsigned int id, const QString& name, EDeckType type, unsigned int battlegroundId)
: mId(id)
, mName(name)
, mType(type)
, mBattlegroundId(battlegroundId)
, mCards()
{

}

void CDeck::setId(unsigned int id)
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

void CDeck::setBattleground(unsigned int battlegroundId)
{
    mBattlegroundId = battlegroundId;
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

unsigned int CDeck::getId() const
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

unsigned int CDeck::getBattlegroundId() const
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
    return !mName.isEmpty() && !mCards.isEmpty() && mCards.first().getType() == ECommanderType;
}

bool CDeck::isEmpty() const
{
    return mCards.isEmpty();
}

