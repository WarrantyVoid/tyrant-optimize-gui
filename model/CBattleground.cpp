#include "CBattleground.h"

const CBattleground CBattleground::INVALID_BATTLEGROUND;

CBattleground::CBattleground()
: mId(0u)
, mName("")
, mDescription("")
, mPicture("")
{
}

void CBattleground::setId(TBattlegroundId id)
{
    mId = id;
}

void CBattleground::setName(const QString &name)
{
    mName = name;
}

void CBattleground::setDescription(const QString &description)
{
    mDescription = description;
}

void CBattleground::setPicture(const QString &picture)
{
    mPicture = picture;
}

TBattlegroundId CBattleground::getId() const
{
    return mId;
}

QString CBattleground::getName() const
{
    return mName;
}

QString CBattleground::getDescription() const
{
    return mDescription;
}

QString CBattleground::getPicture() const
{
    return mPicture;
}

bool CBattleground::isValid() const
{
    return mId > 0 && !mName.isEmpty() && !mDescription.isEmpty() && !mPicture.isEmpty();
}
