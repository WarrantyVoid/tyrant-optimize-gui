#include "CCardSkill.h"

const CCardSkill CCardSkill::INVALID_CARD_SKILL;

CCardSkill::CCardSkill()
: mId("")
, mX(0)
, mY(0)
, mTargetsAll(false)
, mFlags(ECardSkillFlagNone)
{

}

CCardSkill::CCardSkill(const QString &id, int x, int y, bool all, ECardSkillFlags flags)
: mId(id)
, mX(x)
, mY(y)
, mTargetsAll(all)
, mFlags(flags)
{

}

QString CCardSkill::getId() const
{
    return mId;
}

int CCardSkill::getX() const
{
    return mX;
}

ECardFaction CCardSkill::getTargetingFaction() const
{
    switch (mY)
    {
    case EImperialFaction: return EImperialFaction;
    case EBloodthirstyFaction: return EBloodthirstyFaction;
    case EXenoFaction: return EXenoFaction;
    case ERighteousFaction: return ERighteousFaction;
    case ERaiderFaction: return ERaiderFaction;
    default: return ENoFaction; // == all factions
    }
}

bool CCardSkill::isTargetingAll() const
{
    return mTargetsAll;
}

ECardSkillFlags CCardSkill::getFlags() const
{
    return mFlags;
}

bool CCardSkill::isValid() const
{
    return !mId.isEmpty();
}
