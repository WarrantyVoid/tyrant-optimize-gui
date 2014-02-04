#include "xml/CMissionsXmlParser.h"

CMissionsXmlParser::CMissionsXmlParser()
: mMission("mission")
, mCommander("commander")
, mDeck("deck")
, mCard("card")
, mName("name", "")
, mId("id", 0u)
, mBattlegroundId("effect", 0u)
, mDeckCardList()
{

}

bool CMissionsXmlParser::startDocument()
{
    mMission.reset();
    mCommander.reset();
    mDeck.reset();
    mCard.reset();
    mName.reset();
    mId.reset();
    mBattlegroundId.reset();
    mDeckCardList.clear();
    return true;
}

bool CMissionsXmlParser::startElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName, const QXmlAttributes & /*atts*/)
{

    if (mMission.handleStartElement(qName));
    else if (mMission.isOpen())
    {
        if (mName.handleStartElement(qName));
        else if (mId.handleStartElement(qName));
        else if (mBattlegroundId.handleStartElement(qName));
        else if (mCommander.handleStartElement(qName));
        else if (mDeck.handleStartElement(qName));
        else if (mDeck.isOpen())
        {
            if (mCard.handleStartElement(qName)){ };
        }
    }
    return true;
}

bool CMissionsXmlParser::endElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName)
{
    if (mMission.handleEndElement(qName))
    {
        emit missionParsed(
            mId.value(),
            mName.value(),
            EMissionDeckType,
            mBattlegroundId.value(),
            mDeckCardList);
        mName.reset();
        mId.reset();
        mBattlegroundId.reset();
        mDeckCardList.clear();
    }
    else if (mMission.isOpen())
    {
        if (mName.handleEndElement(qName));
        else if (mId.handleEndElement(qName));
        else if (mBattlegroundId.handleEndElement(qName));
        else if (mCommander.handleEndElement(qName));
        else if (mDeck.handleEndElement(qName));
        else if (mDeck.isOpen())
        {
            if (mCard.handleEndElement(qName)){ };
        }
    }
    return true;
}

bool CMissionsXmlParser::characters(const QString &ch)
{
    if (mMission.isOpen())
    {
        if (mName.isOpen())
        {
            mName.setValue(ch.trimmed());
        }
        else if (mId.isOpen())
        {
            bool ok(true);
            mId.setValue(ch.toUInt(&ok));
        }
        else if (mBattlegroundId.isOpen())
        {
            bool ok(true);
            mBattlegroundId.setValue(ch.toUInt(&ok));
        }
        else if (mCommander.isOpen())
        {
            bool ok(true);
            mDeckCardList.push_back(ch.toUInt(&ok));
        }
        else if (mDeck.isOpen())
        {
            if (mCard.isOpen())
            {
                bool ok(true);
                mDeckCardList.push_back(ch.toUInt(&ok));
            }
        }
    }
    return true;
}

bool CMissionsXmlParser::endDocument()
{
    return true;
}
