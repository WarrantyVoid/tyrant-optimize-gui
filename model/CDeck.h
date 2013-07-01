#ifndef CDECK_H
#define CDECK_H

#include <QString>
#include <QList>
#include "CCard.h"

enum EDeckType
{
    EUnknownDeckType,
    EMissionDeckType,
    ERaidDeckType,
    EQuestDeckType,
    ECustomDeckType
};

class CDeck
{
public:
    CDeck();
    CDeck(unsigned int id, const QString& name, EDeckType type, unsigned int battlegroundId = 0u);

public:
    void setId(unsigned int id);
    void setName(const QString& name);
    void setType(EDeckType type);
    void setBattleground(unsigned int battlegroundId);
    void addCard(const CCard& card);
    void replaceCard(int index, const CCard& card);
    void clearCards();

    unsigned int getId() const;
    const QString& getName() const;
    EDeckType getType() const;
    unsigned int getBattlegroundId() const;
    const CCard& getCommander() const;
    const CCard& getLegendary() const;
    const QList<CCard>& getCards() const;
    int getNumCards() const;

    bool isValid() const;
    bool isEmpty() const;

 public:
    static const CDeck INVALID_DECK;

private:
    unsigned int mId;
    QString mName;
    EDeckType mType;
    unsigned int mBattlegroundId;
    QList<CCard> mCards;
};

#endif // CDECK_H
