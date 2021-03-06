#ifndef CRAIDSXMLPARSER_H
#define CRAIDSXMLPARSER_H

#include <QXmlDefaultHandler>
#include "model/CDeck.h"

class CRaidsXmlParser: public QObject, public QXmlDefaultHandler
{

    Q_OBJECT

public:
    CRaidsXmlParser();

signals:
    void raidParsed(TDeckId id, const QString raidName, EDeckType type, TBattlegroundId battlegroundId, const QList<TCardId> &raidDeck);

protected:
    virtual bool startDocument();
    virtual bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts);
    virtual bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName);
    virtual bool characters(const QString & ch);
    virtual bool endDocument();

private:
    bool mIsRaidTagActive;
    bool mIsNameTagActive;
    bool mIsIdTagActive;
    bool mIsCommanderTagActive;
    bool mIsDeckTagActive;
    bool mIsCardTagActive;
    QString mCurRaidName;
    unsigned int mCurRaidId;
    QList<TCardId> mCurRaidDeck;
};

#endif // CRAIDSXMLPARSER_H
