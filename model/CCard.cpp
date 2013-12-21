#include "CCard.h"

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

void CCard::setId(TCardId id)
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

void CCard::addSkill(const CCardSkill &skill)
{
    if (skill.isValid())
    {
        mSkills.push_back(skill);
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

TCardId CCard::getId() const
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

QString CCard::getBaseName() const
{
    QString name = mName;
    name.replace(",", "");
    return name;
}

QString CCard::getUpgradedName() const
{
    QString name = mName;
    name.replace(",", "");
    name.append(QChar('*'));
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

const TSkillList& CCard::getSkills() const
{
    return mSkills;
}

int CCard::getUpgradeLevel() const
{
    return (mSet == EUpgradedSet) ? 1 : mUpgradeLevel;
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
