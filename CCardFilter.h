#ifndef CCARDFILTER_H
#define CCARDFILTER_H

#include "CCardFilterParameters.h"
#include "CCardTable.h"
#include <QTextStream>

class CCardFilter
{
public:
    CCardFilter();

public:
    int updateOwnedCards(const QString &cardsStr, const QString &cardsFilePath);
    void execute(const QString &inputFilePath, const QString &outputFilePath, const CCardFilterParameters &parameters);

protected:
    typedef QPair<CCard, int> TOwnedCard;
    bool readOwnedCard(const QString &cardStr, TOwnedCard &card);
    void writeOwnedCard(QTextStream &stream, const TOwnedCard &card);

private:
    CCardTable &mCards;
};

#endif // CCARDFILTER_H
