#include "CQuestsXmlParser.h"

CQuestsXmlParser::CQuestsXmlParser()
: mIsStepTagActive(false)
, mIsIdTagActive(false)
, mIsBattlegroundIdTagActive(false)
, mIsCommanderTagActive(false)
, mIsDeckTagActive(false)
, mIsCardTagActive(false)
, mCurQuestName("")
, mCurQuestBattleGroundId(0u)
, mCurQuestDeck()
, mIsBattlegroundTagActive(false)
, mIsBgIdTagActive(false)
, mIsNameTagActive(false)
, mIsDescTagActive(false)
, mIsIconTagActive(false)
, mCurBattleground()
{

}

bool CQuestsXmlParser::startDocument()
{
    mIsStepTagActive = false;
    mIsIdTagActive = false;
    mIsBattlegroundIdTagActive = false;
    mIsCommanderTagActive = false;
    mIsDeckTagActive = false;
    mIsCardTagActive = false;
    mCurQuestName = "";
    mCurQuestBattleGroundId = 0u;
    mCurQuestDeck.clear();

    mIsBattlegroundTagActive = false;
    mIsBgIdTagActive = false;
    mIsNameTagActive = false;
    mIsDescTagActive = false;
    mIsIconTagActive = false;
    mCurBattleground = CBattleground::INVALID_BATTLEGROUND;
    return true;
}

bool CQuestsXmlParser::startElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName, const QXmlAttributes & /*atts*/)
{
    if (qName.compare("step") == 0)
    {
        mIsStepTagActive = true;
    }
    else if (mIsStepTagActive)
    {
        if (qName.compare("id") == 0)
        {
            mIsIdTagActive = true;
        }
        else if (qName.compare("battleground_id") == 0)
        {
            mIsBattlegroundIdTagActive = true;
        }
        else if (qName.compare("commander") == 0)
        {
            mIsCommanderTagActive = true;
        }
        else if (qName.compare("deck") == 0)
        {
            mIsDeckTagActive = true;
        }
        else if (qName.compare("card") == 0)
        {
            mIsCardTagActive = true;
        }
    }
    else if (qName.compare("battleground") == 0)
    {
        mIsBattlegroundTagActive = true;
    }
    else if (mIsBattlegroundTagActive)
    {
        if (qName.compare("id") == 0)
        {
            mIsBgIdTagActive = true;
        }
        else if (qName.compare("name") == 0)
        {
            mIsNameTagActive = true;
        }
        else if (qName.compare("desc") == 0)
        {
            mIsDescTagActive = true;
        }
        else if (qName.compare("icon") == 0)
        {
            mIsIconTagActive = true;
        }
    }
    return true;
}

bool CQuestsXmlParser::endElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName)
{  
    if (mIsStepTagActive)
    {
        if (qName.compare("step") == 0)
        {
            emit questParsed(mCurQuestName, EQuestDeckType, mCurQuestBattleGroundId, mCurQuestDeck);
            mIsStepTagActive = false;
            mCurQuestName = "";
            mCurQuestBattleGroundId = 0u;
            mCurQuestDeck.clear();
        }
        else if (qName.compare("id") == 0)
        {
            mIsIdTagActive = false;
        }
        else if (qName.compare("battleground_id") == 0)
        {
            mIsBattlegroundIdTagActive = false;
        }
        else if (qName.compare("commander") == 0)
        {
            mIsCommanderTagActive = false;
        }
        else if (qName.compare("deck") == 0)
        {
            mIsDeckTagActive = false;
        }
        else if (qName.compare("card") == 0)
        {
            mIsCardTagActive = false;
        }
    }
    else if (mIsBattlegroundTagActive)
    {
        if (qName.compare("battleground") == 0)
        {
            emit battlegroundParsed(mCurBattleground);
            mIsBattlegroundTagActive = false;
            mCurBattleground = CBattleground::INVALID_BATTLEGROUND;
        }
        else if (qName.compare("id") == 0)
        {
            mIsBgIdTagActive = false;
        }
        else if (qName.compare("name") == 0)
        {
            mIsNameTagActive = false;
        }
        else if (qName.compare("desc") == 0)
        {
            mIsDescTagActive = false;
        }
        else if (qName.compare("icon") == 0)
        {
            mIsIconTagActive = false;
        }
    }
    return true;
}

bool CQuestsXmlParser::characters(const QString & ch)
{
    if (mIsStepTagActive)
    {
        if (mIsIdTagActive)
        {
            mCurQuestName = QString("Step %1").arg(ch);
        }
        else if (mIsBattlegroundIdTagActive)
        {
            bool ok(true);
            mCurQuestBattleGroundId = ch.toUInt(&ok);
        }
        else if (mIsCommanderTagActive)
        {
            bool ok(true);
            mCurQuestDeck.push_back(ch.toUInt(&ok));
        }
        else if (mIsDeckTagActive && mIsCardTagActive)
        {
            bool ok(true);
            mCurQuestDeck.push_back(ch.toUInt(&ok));
        }
    }
    else if (mIsBattlegroundTagActive)
    {
        if (mIsBgIdTagActive)
        {
            bool ok(true);
            mCurBattleground.setId(ch.toUInt(&ok));
        }
        else if (mIsNameTagActive)
        {
            mCurBattleground.setName(ch);
        }
        else if (mIsDescTagActive)
        {
            mCurBattleground.setDescription(ch);
        }
        else if (mIsIconTagActive)
        {
            mCurBattleground.setPicture(ch);
        }
    }
    return true;
}

bool CQuestsXmlParser::endDocument()
{
    return true;
}
