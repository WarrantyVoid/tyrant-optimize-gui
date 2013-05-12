#include "xml/CRaidsXmlParser.h"

CRaidsXmlParser::CRaidsXmlParser()
: mIsRaidTagActive(false)
, mIsNameTagActive(false)
, mIsCommanderTagActive(false)
, mIsDeckTagActive(false)
, mIsCardTagActive(false)
, mCurRaidName("")
, mCurRaidDeck()
{

}

bool CRaidsXmlParser::startDocument()
{
    mIsRaidTagActive = false;
    mIsNameTagActive = false;
    mIsCommanderTagActive = false;
    mIsDeckTagActive = false;
    mIsCardTagActive = false;
    mCurRaidName = "";
    mCurRaidDeck.clear();
    return true;
}

bool CRaidsXmlParser::startElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName, const QXmlAttributes & /*atts*/)
{
    if (qName.compare("raid") == 0)
    {
        mIsRaidTagActive = true;
    }
    else if (mIsRaidTagActive)
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

bool CRaidsXmlParser::endElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName)
{
    if (mIsRaidTagActive)
    {
        if (qName.compare("raid") == 0)
        {
            emit raidParsed(mCurRaidName, ERaidDeckType, 0u, mCurRaidDeck);
            mIsRaidTagActive = false;
            mCurRaidName = "";
            mCurRaidDeck.clear();
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

bool CRaidsXmlParser::characters(const QString & ch)
{
    if (mIsRaidTagActive)
    {
        if (mIsNameTagActive)
        {
            mCurRaidName = ch;
        }
        else if (mIsCommanderTagActive)
        {
            bool ok(true);
            mCurRaidDeck.push_back(ch.toUInt(&ok));
        }
        else if (mIsDeckTagActive && mIsCardTagActive)
        {
            bool ok(true);
            mCurRaidDeck.push_back(ch.toUInt(&ok));
        }
    }
    return true;
}

bool CRaidsXmlParser::endDocument()
{
    return true;
}
