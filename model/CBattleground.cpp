#include "CBattleground.h"

const CBattleground CBattleground::INVALID_BATTLEGROUND;

CBattleground::CBattleground()
: mName("")
, mDescription("")
, mPicture("")
{
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
    return !mName.isEmpty() && !mDescription.isEmpty() && !mPicture.isEmpty();
}
