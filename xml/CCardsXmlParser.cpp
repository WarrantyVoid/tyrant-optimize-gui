#include "xml/CCardsXmlParser.h"

CCardsXmlParser::CCardsXmlParser()
: mIsUnitTagActive(false)
, mIsIdTagActive(false)
, mIsNameTagActive(false)
, mIsPictureTagActive(false)
, mIsRarityTagActive(false)
, mIsTypeTagActive(false)
, mIsSetTagActive(false)
, mIsCostTagActive(false)
, mIsUniqueTagActive(false)
, mIsAttackTagActive(false)
, mIsHealthTagActive(false)
, mCurCard()
, mSkillTypeTagActive(false)
, mSkillIdTagActive(false)
, mSkillNameTagActive(false)
, mSkillIconTagActive(false)
, mSkillDescTagActive(false)
, mCurSkill()
, mCardSetTagActive(false)
, mCardSetIdTagActive(false)
, mCardSetIconTagActive(false)
, mCurrentCardSet(EUnknownSet)
, mCurrentCardSetPicture("")
{

}

bool CCardsXmlParser::startDocument()
{
    mIsUnitTagActive = false;
    mIsIdTagActive = false;
    mIsNameTagActive = false;
    mIsPictureTagActive = false;
    mIsRarityTagActive = false;
    mIsTypeTagActive = false;
    mIsSetTagActive = false;
    mIsCostTagActive = false;
    mIsUniqueTagActive = false;
    mIsAttackTagActive = false;
    mIsHealthTagActive = false;
    mCurCard = CCard::INVALID_CARD;

    mSkillTypeTagActive = false;
    mSkillIdTagActive = false;
    mSkillNameTagActive = false;
    mSkillIconTagActive = false;
    mSkillDescTagActive = false;
    mCurSkill = CSkill::INVALID_SKILL;

    mCardSetTagActive = false;
    mCardSetIdTagActive = false;
    mCardSetIconTagActive = false;
    mCurrentCardSet = EUnknownSet;
    mCurrentCardSetPicture = "";

    return true;
}

bool CCardsXmlParser::startElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName, const QXmlAttributes & atts)
{
    if (qName.compare("unit") == 0)
    {
        mIsUnitTagActive = true;
        mCurCard.setFaction(ENoFaction);
    }
    else if (mIsUnitTagActive)
    {
        if (qName.compare("id") == 0)
        {
            mIsIdTagActive = true;
        }
        else if (qName.compare("name") == 0)
        {
            mIsNameTagActive = true;
        }
        else if (qName.compare("picture") == 0)
        {
            mIsPictureTagActive = true;
        }        
        else if (qName.compare("rarity") == 0)
        {
            mIsRarityTagActive = true;
        }
        else if (qName.compare("type") == 0)
        {
            mIsTypeTagActive = true;
        }
        else if (qName.compare("set") == 0)
        {
            mIsSetTagActive = true;
        }
        else if (qName.compare("upgraded_card") == 0)
        {
            //mCurCard.setUpgradeLevel(0);
        }
        else if (qName.compare("base_card") == 0)
        {
            //mCurCard.setUpgradeLevel(1);
        }
        else if (qName.compare("skill") == 0)
        {
            QString id = "";
            int x = -1;
            int y = -1;
            bool all = false;
            ECardSkillFlags flags = ECardSkillFlagNone;
            for (int i = 0; i < atts.count(); ++i)
            {
                if (atts.qName(i).compare("id") == 0)
                {
                    id = atts.value(i);
                }
                if (atts.qName(i).compare("x") == 0)
                {
                    bool ok(true);
                    x = atts.value(i).toInt(&ok);
                }
                if (atts.qName(i).compare("y") == 0)
                {
                    bool ok(true);
                    y = atts.value(i).toInt(&ok);
                }
                if (atts.qName(i).compare("all") == 0)
                {
                    atts.value(i).toInt(&all);
                }
                if (atts.qName(i).compare("attacked") == 0)
                {
                    bool ok(true);
                    if (atts.value(i).toInt(&ok) == 1)
                    {
                       flags |= ECardSkillFlagOnAttacked;
                    }
                }
                if (atts.qName(i).compare("kill") == 0)
                {
                    bool ok(true);
                    if (atts.value(i).toInt(&ok) == 1)
                    {
                       flags |= ECardSkillFlagOnKill;
                    }
                }
                if (atts.qName(i).compare("played") == 0)
                {
                    bool ok(true);
                    if (atts.value(i).toInt(&ok) == 1)
                    {
                       flags |= ECardSkillFlagOnPlay;
                    }
                }
                if (atts.qName(i).compare("died") == 0)
                {
                    bool ok(true);
                    if (atts.value(i).toInt(&ok) == 1)
                    {
                       flags |= ECardSkillFlagOnDeath;
                    }
                }
            }
            mCurCard.addSkill(CCardSkill(id, x, y, all, flags));
        }
        else if (qName.compare("cost") == 0)
        {
            mIsCostTagActive = true;
        }
        else if (qName.compare("unique") == 0)
        {
            mIsUniqueTagActive = true;
        }
        else if (qName.compare("attack") == 0)
        {
            mIsAttackTagActive = true;
        }
        else if (qName.compare("health") == 0)
        {
            mIsHealthTagActive = true;
        }
    }
    else if (qName.compare("skillType") == 0)
    {
        mSkillTypeTagActive = true;
    }
    else if (mSkillTypeTagActive)
    {
        if (qName.compare("id") == 0)
        {
            mSkillIdTagActive = true;
        }
        else if (qName.compare("name") == 0)
        {
            mSkillNameTagActive = true;
        }
        else if (qName.compare("icon") == 0)
        {
            mSkillIconTagActive = true;
        }
        else if (qName.compare("desc") == 0)
        {
            mSkillDescTagActive = true;
        }
    }
    else if (qName.compare("cardSet") == 0)
    {
        mCardSetTagActive = true;
    }
    else if (mCardSetTagActive)
    {
        if (qName.compare("id") == 0)
        {
            mCardSetIdTagActive = true;
        }
        else if (qName.compare("icon") == 0)
        {
            mCardSetIconTagActive = true;
        }
    }
    return true;
}

bool CCardsXmlParser::endElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName)
{
    if (mIsUnitTagActive)
    {
        if (qName.compare("unit") == 0)
        {
            emit cardParsed(mCurCard);
            mIsUnitTagActive = false;
            mCurCard = CCard::INVALID_CARD;
        }
        else if (qName.compare("id") == 0)
        {
            mIsIdTagActive = false;
        }
        else if (qName.compare("name") == 0)
        {
            mIsNameTagActive = false;
        }
        else if (qName.compare("picture") == 0)
        {
            mIsPictureTagActive = false;
        }
        else if (qName.compare("rarity") == 0)
        {
            mIsRarityTagActive = false;
        }
        else if (qName.compare("type") == 0)
        {
            mIsTypeTagActive = false;
        }
        else if (qName.compare("set") == 0)
        {
            mIsSetTagActive = false;
        }
        else if (qName.compare("cost") == 0)
        {
            mIsCostTagActive = false;
        }
        else if (qName.compare("unique") == 0)
        {
            mIsUniqueTagActive = false;
        }
        else if (qName.compare("attack") == 0)
        {
            mIsAttackTagActive = false;
        }
        else if (qName.compare("health") == 0)
        {
            mIsHealthTagActive = false;
        }
    }
    else if (mSkillTypeTagActive)
    {
        if (qName.compare("skillType") == 0)
        {
            emit skillParsed(mCurSkill);
            mSkillTypeTagActive = false;
            mCurSkill = CSkill::INVALID_SKILL;
        }
        else if (qName.compare("id") == 0)
        {
            mSkillIdTagActive = false;
        }
        else if (qName.compare("name") == 0)
        {
            mSkillNameTagActive = false;
        }
        else if (qName.compare("icon") == 0)
        {
            mSkillIconTagActive = false;
        }
        else if (qName.compare("desc") == 0)
        {
            mSkillDescTagActive = false;
        }
    }
    else if (mCardSetTagActive)
    {
        if (qName.compare("cardSet") == 0)
        {
            emit cardSetParsed(mCurrentCardSet, mCurrentCardSetPicture);
            mCardSetTagActive = false;
            mCurrentCardSet = EUnknownSet;
            mCurrentCardSetPicture = "";
        }
        else if (qName.compare("id") == 0)
        {
            mCardSetIdTagActive = false;
        }
        else if (qName.compare("icon") == 0)
        {
            mCardSetIconTagActive = false;
        }
    }
    return true;
}

bool CCardsXmlParser::characters(const QString & ch)
{
    if (mIsUnitTagActive)
    {
        if (mIsIdTagActive)
        {
            bool ok(true);
            mCurCard.setId(ch.toUInt(&ok));
        }
        else if (mIsNameTagActive)
        {
            mCurCard.setName(ch);
        }
        else if (mIsPictureTagActive)
        {
            mCurCard.setPicture(ch);
        }
        else if (mIsRarityTagActive)
        {
            bool ok(true);
            mCurCard.setRarity((ECardRarity)ch.toUInt(&ok));
        }
        else if (mIsTypeTagActive)
        {
            bool ok(true);
            mCurCard.setFaction((ECardFaction)ch.toUInt(&ok));
        }
        else if (mIsSetTagActive)
        {
            bool ok(true);
            mCurCard.setSet((ECardSet)ch.toUInt(&ok));
        }
        else if (mIsCostTagActive)
        {
            bool ok(true);
            mCurCard.setDelay(ch.toInt(&ok));
        }
        else if (mIsUniqueTagActive)
        {
            bool ok(true);
            mCurCard.setUnique(ch.toInt(&ok) == 1);
        }
        else if (mIsAttackTagActive)
        {
            bool ok(true);
            mCurCard.setAttack(ch.toInt(&ok));
        }
        else if (mIsHealthTagActive)
        {
            bool ok(true);
            mCurCard.setHealth(ch.toInt(&ok));
        }
    }
    else if (mSkillTypeTagActive)
    {
        if (mSkillIdTagActive)
        {
            mCurSkill.setId(ch);
        }
        else if (mSkillNameTagActive)
        {
            mCurSkill.setName(ch);
        }
        else if (mSkillIconTagActive)
        {
            mCurSkill.setPicture(ch);
        }
        else if (mSkillDescTagActive)
        {
            mCurSkill.setDescription(ch);
        }
    }
    else if(mCardSetTagActive)
    {
        if (mCardSetIdTagActive)
        {
            bool ok(true);
            mCurrentCardSet = (ECardSet)ch.toInt(&ok);
        }
        else if (mCardSetIconTagActive)
        {
            mCurrentCardSetPicture = ch;
        }
    }
    return true;
}

bool CCardsXmlParser::endDocument()
{
    return true;
}
