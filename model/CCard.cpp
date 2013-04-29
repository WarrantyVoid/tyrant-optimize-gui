#include "CCard.h"

CCardSkill::CCardSkill(const QString &id, int x, int y, bool all, unsigned int flags)
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

unsigned int CCardSkill::getFlags() const
{
    return mFlags;
}

const CCard CCard::INVALID_CARD;
const int CCard::UPGRADE_LEVEL_NONE = -1;

CCard::CCard()
: mId(0)
, mName("")
, mLwCsName("")
, mPicture("")
, mRarity(EUnknownRarity)
, mFaction(EUnknownFaction)
, mSet(EUnknownSet)
, mSkills()
, mUpgradeLevel(UPGRADE_LEVEL_NONE)
, mDelay(-1)
, mAttack(-1)
, mHealth(-1)
, mIsUnique(false)
{

}

void CCard::setId(unsigned int id)
{
    mId = id;
}

void CCard::setName(const QString &name)
{
    mName = name;
    mLwCsName = getName().toLower();
}

void CCard::setPicture(const QString &picture)
{
    mPicture = picture;
}

void CCard::setRarity(ECardRarity rarity)
{
    mRarity = rarity;
}

void CCard::setFaction(ECardFaction type)
{
    mFaction = type;
}

void  CCard::setSet(ECardSet set)
{
    mSet = set;
}

void CCard::clearSkills()
{
    mSkills.clear();
}

void CCard::addSkill(const QString &str, int x, int y, bool all, unsigned int flags)
{
    if (!str.isEmpty())
    {
        mSkills.push_back(CCardSkill(str, x, y, all, flags));
    }
}

void CCard::setUpgradeLevel(int level)
{
    mUpgradeLevel = level;
}

void CCard::setDelay(int delay)
{
    mDelay = delay;
}

void CCard::setUnique(bool isUnique)
{
    mIsUnique = isUnique;
}

void CCard::setAttack(int attack)
{
    mAttack = attack;
}

void CCard::setHealth(int health)
{
    mHealth = health;
}

unsigned int CCard::getId() const
{
    return mId;
}

QString CCard::getName() const
{
    QString name = mName;
    name.replace(",", "");
    if (getUpgradeLevel() > 0)
    {
        name.append(QChar('*'));
    }
    return name;
}

QString CCard::getLwCsName() const
{
    return mLwCsName;
}

QString CCard::getPicture() const
{
    return mPicture;
}

ECardRarity CCard::getRarity() const
{
    return mRarity;
}

ECardFaction CCard::getFaction() const
{
    return mFaction;
}

ECardType CCard::getType() const
{
    if (mId >= 4000)
    {
        // Extended assault ID
        return EAssaultType;
    }
    else if (mId >= EActionType)
    {
        return EActionType;
    }
    else if (mId >= EStructureType)
    {
        return EStructureType;
    }
    else if (mId >= ECommanderType)
    {
        return ECommanderType;
    }
    else if (mId >= EAssaultType)
    {
        return EAssaultType;
    }
    else
    {
        return EUnknownType;
    }
}

ECardSet CCard::getSet() const
{
    return mSet;
}

const TCardSkills& CCard::getSkills() const
{
    return mSkills;
}

int CCard::getUpgradeLevel() const
{
    return (mSet == EUpgradedSet) ? 1 : UPGRADE_LEVEL_NONE;
    //return mUpgradeLevel;
}

int CCard::getDelay() const
{
    return mDelay;
}

bool CCard::isUnique() const
{
    return mRarity == ELegendaryRarity ? false : mIsUnique;
}

int CCard::getAttack() const
{
    return mAttack;
}

bool CCard::hasAttack() const
{
    return getType() == EAssaultType && mAttack > - 1;
}

int CCard::getHealth() const
{
    return mHealth;
}

bool  CCard::hasHealth() const
{
    return getType() != EActionType && mHealth > -1;
}

bool CCard::isValid() const
{
    return mId > 0
        && !mName.isEmpty()
        && !mPicture.isEmpty()
        && mPicture.compare("loading.jpg") != 0
        && mRarity != EUnknownRarity
        && mFaction != EUnknownFaction;
        //&& mSet != EUnknownSet;
}
