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
    void questParsed(const QString &questName, EDeckType type, const QList<unsigned int> &questDeck);
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
    bool mIsCommanderTagActive;
    bool mIsDeckTagActive;
    bool mIsCardTagActive;
    QString mCurQuestName;
    QList<unsigned int> mCurQuestDeck;

    bool mIsBattlegroundTagActive;
    bool mIsNameTagActive;
    bool mIsDescTagActive;
    bool mIsIconTagActive;
    CBattleground mCurBattleground;
};

#endif // CQUESTSXMLPARSER_H
