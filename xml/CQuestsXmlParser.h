#ifndef CQUESTSXMLPARSER_H
#define CQUESTSXMLPARSER_H

#include <QXmlDefaultHandler>
#include "model/CBattleground.h"
#include "model/CDeck.h"

class CQuestsXmlParser : public QObject, public QXmlDefaultHandler
{
    Q_OBJECT

public:
    CQuestsXmlParser();

signals:
    void questParsed(TDeckId id, const QString &questName, EDeckType type, TBattlegroundId battlegroundId, const QList<TCardId> &questDeck);
    void battlegroundParsed(const CBattleground &battleground);

protected:
    virtual bool startDocument();
    virtual bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts);
    virtual bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName);
    virtual bool characters(const QString & ch);
    virtual bool endDocument();

private:
    bool mIsStepTagActive;
    bool mIsIdTagActive;
    bool mIsBattlegroundIdTagActive;
    bool mIsCommanderTagActive;
    bool mIsDeckTagActive;
    bool mIsCardTagActive;
    QString mCurQuestName;
    unsigned int mCurQuestId;
    unsigned int mCurQuestBattleGroundId;
    QList<TCardId> mCurQuestDeck;

    bool mIsBattlegroundTagActive;
    bool mIsBgIdTagActive;
    bool mIsNameTagActive;
    bool mIsDescTagActive;
    bool mIsIconTagActive;
    CBattleground mCurBattleground;
};

#endif // CQUESTSXMLPARSER_H
