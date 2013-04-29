#include "xml/CAchievementsXmlParser.h"

CAchievementsXmlParser::CAchievementsXmlParser()
: mIsAchievementTagActive(false)
, mIsIdTagActive(false)
, mIsNameTagActive(false)
, mIsDescTagActive(false)
, mCurAchievement()
{
}

bool CAchievementsXmlParser::startDocument()
{
    mIsAchievementTagActive = false;
    mIsIdTagActive = false;
    mIsNameTagActive = false;
    mIsDescTagActive = false;
    mCurAchievement = CAchievement::INVALID_ACHIEVEMENT;
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
        if (qName.compare("id") == 0)
        {
            mIsIdTagActive = true;
        }
        else if (qName.compare("name") == 0)
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
            emit achievementParsed(mCurAchievement);
            mIsAchievementTagActive = false;
            mCurAchievement = CAchievement::INVALID_ACHIEVEMENT;
        }
        else if (qName.compare("id") == 0)
        {
            mIsIdTagActive = false;
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
        if (mIsIdTagActive)
        {
            bool ok(true);
            mCurAchievement.setId(ch.toUInt(&ok));
        }
        else if (mIsNameTagActive)
        {
            mCurAchievement.setName(ch);
        }
        else if (mIsDescTagActive)
        {
           mCurAchievement.setDescription(ch);
        }
    }
    return true;
}

bool CAchievementsXmlParser::endDocument()
{
    return true;
}
