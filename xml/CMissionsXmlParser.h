#ifndef CMISSIONSXMLPARSER_H
#define CMISSIONSXMLPARSER_H

#include <QXmlDefaultHandler>
#include "model/CDeck.h"
#include "CXmlSaxValue.h"

class CMissionsXmlParser : public QObject, public QXmlDefaultHandler
{

    Q_OBJECT

public:
    CMissionsXmlParser();

signals:
    void missionParsed(TDeckId id, const QString &missionName, EDeckType type, TBattlegroundId battlegroundId, const QList<TCardId> &missionDeck);

protected:
    virtual bool startDocument();
    virtual bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts);
    virtual bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName);
    virtual bool characters(const QString & ch);
    virtual bool endDocument();

private:
    CXmlSaxValue<> mMission;
    CXmlSaxValue<> mCommander;
    CXmlSaxValue<> mDeck;
    CXmlSaxValue<> mCard;
    CXmlSaxValue<QString> mName;
    CXmlSaxValue<unsigned int> mId;
    CXmlSaxValue<unsigned int> mBattlegroundId;
    QList<TCardId> mDeckCardList;
};

#endif // CMISSIONSXMLPARSER_H
