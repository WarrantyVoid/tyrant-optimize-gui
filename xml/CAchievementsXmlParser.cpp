#include "xml/CAchievementsXmlParser.h"

CAchievementsXmlParser::CAchievementsXmlParser()
: mIsAchievementTagActive(false)
, mIsNameTagActive(false)
, mIsDescTagActive(false)
, mCurAchievementName("")
, mCurAchievementDesc("")
{
}

bool CAchievementsXmlParser::startDocument()
{
    mIsAchievementTagActive = false;
    mIsNameTagActive = false;
    mIsDescTagActive = false;
    mCurAchievementName = "";
    mCurAchievementDesc = "";
    return true;
}

bool CAchievementsXmlParser::startElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName, const QXmlAttributes & /*atts*/)
{
    if (qName.compare("achievement") == 0)
    {
        mIsAchievementTagActive = true;
    }
    else if (mIsAchievementTagActive)
    {
        if (qName.compare("name") == 0)
        {
            mIsNameTagActive = true;
        }
        else if (qName.compare("desc") == 0)
        {
            mIsDescTagActive = true;
        }
    }
    return true;
}

bool CAchievementsXmlParser::endElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName)
{
    if (mIsAchievementTagActive)
    {
        if (qName.compare("achievement") == 0)
        {
            emit achievementParsed(mCurAchievementName, mCurAchievementDesc);
            mIsAchievementTagActive = false;
            mCurAchievementName = "";
            mCurAchievementDesc = "";
        }
        else if (qName.compare("name") == 0)
        {
            mIsNameTagActive = false;
        }
        else if (qName.compare("desc") == 0)
        {
            mIsDescTagActive = false;
        }
    }
    return true;
}

bool CAchievementsXmlParser::characters(const QString & ch)
{
    if (mIsAchievementTagActive)
    {
        if (mIsNameTagActive)
        {
            mCurAchievementName = ch;
        }
        else if (mIsDescTagActive)
        {
            mCurAchievementDesc = ch;
        }
    }
    return true;
}

bool CAchievementsXmlParser::endDocument()
{
    return true;
}
