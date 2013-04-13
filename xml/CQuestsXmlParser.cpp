#include "CQuestsXmlParser.h"

CQuestsXmlParser::CQuestsXmlParser()
: mIsBattlegroundTagActive(false)
, mIsNameTagActive(false)
, mIsDescTagActive(false)
, mIsIconTagActive(false)
, mCurBattleground()
{

}

bool CQuestsXmlParser::startDocument()
{
    mIsBattlegroundTagActive = false;
    mIsNameTagActive = false;
    mIsDescTagActive = false;
    mIsIconTagActive = false;
    mCurBattleground = CBattleground::INVALID_BATTLEGROUND;
    return true;
}

bool CQuestsXmlParser::startElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName, const QXmlAttributes & /*atts*/)
{
    if (qName.compare("battleground") == 0)
    {
        mIsBattlegroundTagActive = true;
    }
    else if (mIsBattlegroundTagActive)
    {
        if (qName.compare("name") == 0)
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
    if (mIsBattlegroundTagActive)
    {
        if (qName.compare("battleground") == 0)
        {
            emit battlegroundParsed(mCurBattleground);
            mIsBattlegroundTagActive = false;
            mCurBattleground = CBattleground::INVALID_BATTLEGROUND;
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
    if (mIsBattlegroundTagActive)
    {
        if (mIsNameTagActive)
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
