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
    CDeck(const QString& name, EDeckType type);

public:
    void setName(const QString& name);
    void setType(EDeckType type);
    void addCard(const CCard& card);
    void replaceCard(int index, const CCard& card);
    void clearCards();    

    const QString& getName() const;
    EDeckType getType() const;
    const CCard& getCommander() const;
    const QList<CCard>& getCards() const;
    int getNumCards() const;

    bool isValid() const;
    bool isEmpty() const;

 public:
    static const CDeck INVALID_DECK;

private:
    QString mName;
    EDeckType mType;
    QList<CCard> mCards;
};

#endif // CDECK_H
