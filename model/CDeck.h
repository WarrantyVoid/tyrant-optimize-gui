#ifndef CDECK_H
#define CDECK_H

#include <QString>
#include <QList>
#include "CCard.h"
#include "CBattleground.h"

enum EDeckType
{
    EUnknownDeckType,
    EMissionDeckType,
    ERaidDeckType,
    EQuestDeckType,
    ECustomDeckType
};

typedef unsigned int TDeckId;

class CDeck
{
public:
    CDeck();
    CDeck(TDeckId id, const QString& name, EDeckType type, TBattlegroundId battlegroundId = 0u);

public:
    void setId(TDeckId id);
    void setName(const QString& name);
    void setType(EDeckType type);
    void setBattleground(TBattlegroundId battlegroundId);
    void setCommander(const CCard& card);
    void addCard(const CCard& card);
    void replaceCard(int index, const CCard& card);
    void trimCards(int maxCards);
    void clearCards();

    TDeckId getId() const;
    const QString& getName() const;
    EDeckType getType() const;
    TBattlegroundId getBattlegroundId() const;
    const CCard& getCommander() const;
    const CCard& getLegendary() const;
    const TCardList& getCards() const;
    int getNumCards() const;

    bool isValid() const;
    bool isEmpty() const;

 public:
    static const CDeck INVALID_DECK;

private:
    TDeckId mId;
    QString mName;
    EDeckType mType;
    TBattlegroundId mBattlegroundId;
    TCardList mCards;
};

#endif // CDECK_H
