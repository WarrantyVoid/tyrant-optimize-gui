#ifndef CCARDSXMLPARSER_H
#define CCARDSXMLPARSER_H

#include <QXmlDefaultHandler>
#include "model/CCard.h"
#include "model/CSkill.h"

class CCardsXmlParser : public QObject, public QXmlDefaultHandler
{

    Q_OBJECT

public:
    CCardsXmlParser();

signals:
    void cardParsed(const CCard &card);
    void skillParsed(const CSkill &skill);
    void cardSetParsed(ECardSet cardSet, const QString &picture);

protected:
    virtual bool startDocument();
    virtual bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts);
    virtual bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName);
    virtual bool characters(const QString & ch);
    virtual bool endDocument();

private:
    bool mIsUnitTagActive;
    bool mIsIdTagActive;
    bool mIsNameTagActive;
    bool mIsPictureTagActive;
    bool mIsRarityTagActive;
    bool mIsTypeTagActive;
    bool mIsSetTagActive;
    bool mIsCostTagActive;
    bool mIsUniqueTagActive;
    bool mIsAttackTagActive;
    bool mIsHealthTagActive;
    CCard mCurCard;

    bool mSkillTypeTagActive;
    bool mSkillIdTagActive;
    bool mSkillNameTagActive;
    bool mSkillIconTagActive;
    bool mSkillDescTagActive;
    CSkill mCurSkill;

    bool mCardSetTagActive;
    bool mCardSetIdTagActive;
    bool mCardSetIconTagActive;
    ECardSet mCurrentCardSet;
    QString mCurrentCardSetPicture;
};

#endif // CCARDSXMLPARSER_H
