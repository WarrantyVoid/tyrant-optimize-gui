#include "CSkill.h"
#include "CCardTable.h"

const CSkill CSkill::INVALID_SKILL;

CSkill::CSkill()
: mId("")
, mName("")
, mPicture("")
, mDescription("")
{
}

void CSkill::setId(const QString& id)
{
    mId = id;
}

void CSkill::setName(const QString& name)
{
    mName = name;
}

void CSkill::setPicture(const QString& picture)
{
    mPicture = picture;
}

void CSkill::setDescription(const QString& description)
{
    mDescription = description;
}

QString CSkill::getId() const
{
    return mId;
}

QString CSkill::getName() const
{
    return mName;
}

QString CSkill::getPicture() const
{
    return mPicture;
}

QString CSkill::getDescription() const
{
    return mDescription;
}

QString CSkill::makeSignature(const CCardSkill& skillParam, bool linkSummon) const
{
    QString target = "";
    switch(skillParam.getTargetingFaction())
    {
    case EImperialFaction: target = " Imperial"; break;
    case EBloodthirstyFaction: target = " Bloodthirsty"; break;
    case EXenoFaction: target = " Xeno"; break;
    case ERighteousFaction: target = " Righteous"; break;
    case ERaiderFaction: target = " Raider"; break;
    default: break;
    }

    QString x = "";
    if (skillParam.getX() > 0)
    {
        if (isSummon())
        {
            const CCard& card = CCardTable::getCardTable().getCardForId(skillParam.getX());
            if (card.isValid())
            {
                if (linkSummon)
                {
                    x = QString(" <a href='#summon'>%1</a>").arg(card.getName());
                }
                else
                {
                    x = QString(" %1").arg(card.getName());
                }
            }
        }
        else
        {
            x = QString(" %1").arg(skillParam.getX());
        }
    }

    QString mods = "";
    ECardSkillFlags flags = skillParam.getFlags();
    if (flags != 0)
    {
        QStringList modList;
        if (flags.testFlag(ECardSkillFlagOnPlay))
        {
            modList.append("Play");
        }
        if (flags.testFlag(ECardSkillFlagOnAttacked))
        {
            modList.append("Attacked");
        }
        if (flags.testFlag(ECardSkillFlagOnKill))
        {
            modList.append("Kill");
        }
        if (flags.testFlag(ECardSkillFlagOnDeath))
        {
            modList.append("Death");
        }
        mods = QString(" on %1").arg(modList.join("/"));
    }

    QString sig("%1%2%3%4%5");
    return sig.arg(getName())
       .arg(skillParam.isTargetingAll() ? " All" : "")
       .arg(target)
       .arg(x)
       .arg(mods);
}

bool CSkill::isSummon() const
{
    return mId.compare("summon") == 0;
}

bool CSkill::isValid() const
{
    return !mId.isEmpty()
        && !mName.isEmpty()
        && !mPicture.isEmpty()
        && !mDescription.isEmpty();
}
