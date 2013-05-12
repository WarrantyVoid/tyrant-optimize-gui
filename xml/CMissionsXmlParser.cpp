#include "xml/CMissionsXmlParser.h"

CMissionsXmlParser::CMissionsXmlParser()
: mIsMissionTagActive(false)
, mIsNameTagActive(false)
, mIsCommanderTagActive(false)
, mIsDeckTagActive(false)
, mIsCardTagActive(false)
, mCurMissionName("")
, mCurMissionDeck()
{

}

bool CMissionsXmlParser::startDocument()
{
    mIsMissionTagActive = false;
    mIsNameTagActive = false;
    mIsCommanderTagActive = false;
    mIsDeckTagActive = false;
    mIsCardTagActive = false;
    mCurMissionName = "";
    mCurMissionDeck.clear();
    return true;
}

bool CMissionsXmlParser::startElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName, const QXmlAttributes & /*atts*/)
{
    if (qName.compare("mission") == 0)
    {
        mIsMissionTagActive = true;
    }
    else if (mIsMissionTagActive)
    {
        if (qName.compare("name") == 0)
        {
            mIsNameTagActive = true;
        }
        else if (qName.compare("commander") == 0)
        {
            mIsCommanderTagActive = true;
        }
        else if (qName.compare("deck") == 0)
        {
            mIsDeckTagActive = true;
        }
        else if (mIsDeckTagActive)
        {
            if (qName.compare("card") == 0)
            {
                mIsCardTagActive = true;
            }
        }
    }
    return true;
}

bool CMissionsXmlParser::endElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName)
{
    if (mIsMissionTagActive)
    {
        if (qName.compare("mission") == 0)
        {
            emit missionParsed(mCurMissionName, EMissionDeckType, 0u,  mCurMissionDeck);
            mIsMissionTagActive = false;
            mCurMissionName = "";
            mCurMissionDeck.clear();
        }
        else if (qName.compare("name") == 0)
        {
            mIsNameTagActive = false;
        }
        else if (qName.compare("commander") == 0)
        {
            mIsCommanderTagActive = false;
        }
        else if (qName.compare("deck") == 0)
        {
            mIsDeckTagActive = false;
        }
        else if (mIsDeckTagActive)
        {
            if (qName.compare("card") == 0)
            {
                mIsCardTagActive = false;
            }
        }
    }
    return true;
}

bool CMissionsXmlParser::characters(const QString & ch)
{
    if (mIsMissionTagActive)
    {
        if (mIsNameTagActive)
        {
            mCurMissionName = ch;
        }
        else if (mIsCommanderTagActive)
        {
            bool ok(true);
            mCurMissionDeck.push_back(ch.toUInt(&ok));
        }
        else if (mIsDeckTagActive && mIsCardTagActive)
        {
            bool ok(true);
            mCurMissionDeck.push_back(ch.toUInt(&ok));
        }
    }
    return true;
}

bool CMissionsXmlParser::endDocument()
{
    return true;
}
