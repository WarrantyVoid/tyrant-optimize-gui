#ifndef CCARDFILTER_H
#define CCARDFILTER_H

#include "CCardFilterParameters.h"
#include "model/CCardTable.h"
#include <QTextStream>

struct SOwnedCardStatistics
{
    int numCards;
    int numCopies;
};

class CCardFilter
{
public:
    CCardFilter();

public:

    SOwnedCardStatistics writeOwnedCardsToFile(const QString &filePath, const QList<TOwnedCard> &cards);
    SOwnedCardStatistics readOwnedCardsFromFile(const QString &filePath, QList<TOwnedCard> &cards);
    SOwnedCardStatistics readOwnedCardsFromClipboard(const QString &clipboardStr, QList<TOwnedCard> &cards);

    void execute(const QList<TOwnedCard> &inputCards, QList<TOwnedCard> &outputCards, const CCardFilterParameters &parameters);

protected:
    bool readOwnedCard(const QString &cardStr, TOwnedCard &card);
    void writeOwnedCard(QTextStream &stream, const TOwnedCard &card);

private:
    CCardTable &mCards;
};

#endif // CCARDFILTER_H
