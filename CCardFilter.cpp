#include "CCardFilter.h"
#include <QFile>

CCardFilter::CCardFilter()
: mCards(CCardTable::getCardTable())
{
}

int CCardFilter::updateOwnedCards(const QString &cardsStr, const QString &cardsPath)
{
    int numOwned = 0;
    QList<TOwnedCard> ownedCards;
    if (!cardsStr.isEmpty() && cardsStr.length() < 1024 * 1024)
    {
        QStringList cardsSplit = cardsStr.split("\n");
        for (QStringList::const_iterator i = cardsSplit.begin(); i != cardsSplit.end(); ++i)
        {
            const QString &curCardStr = *i;
            TOwnedCard curOwned;
            if (readOwnedCard(curCardStr, curOwned))
            {
                ownedCards.push_back(curOwned);
            }
        }
    }

    if (!ownedCards.isEmpty())
    {
        QFile outputFile(cardsPath);
        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            QTextStream outStream(&outputFile);
            for (QList<TOwnedCard>::const_iterator i = ownedCards.begin(); i != ownedCards.end(); ++i)
            {
                numOwned += (*i).second;
                writeOwnedCard(outStream, *i);
            }
        }
    }
    return numOwned;
}

void CCardFilter::execute(const QString &inputFilePath, const QString &outputFilePath, const CCardFilterParameters &parameters)
{
    QFile inputFile(inputFilePath);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QList<TOwnedCard> ownedCards;
        while (!inputFile.atEnd())
        {
            QString curCardStr(inputFile.readLine());
            TOwnedCard curOwned;
            if (readOwnedCard(curCardStr, curOwned) && parameters.checkCard(curOwned.first, curOwned.second))
            {
                ownedCards.push_back(curOwned);
            }
        }

        QFile outputFile(outputFilePath);
        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            QTextStream outStream(&outputFile);
            for (QList<TOwnedCard>::const_iterator i = ownedCards.begin(); i != ownedCards.end(); ++i)
            {
                writeOwnedCard(outStream, *i);
            }
        }
    }
}

bool CCardFilter::readOwnedCard(const QString &cardStr, TOwnedCard &card)
{
    QStringList cardSplit = cardStr.split(QRegExp("\\[|\\]|\\(|\\)"), QString::SkipEmptyParts);
    if (cardSplit.size() >= 3)
    {
        bool okId(true);
        bool okNum(true);
        unsigned int cardId = cardSplit.at(0).toUInt(&okId);
        card.second = cardSplit.at(2).toInt(&okNum);
        if (okId && okNum)
        {
            card.first = mCards.getCardForId(cardId);
            return card.first.isValid();
        }
    }
    return false;
}

void CCardFilter::writeOwnedCard(QTextStream &stream, const TOwnedCard &card)
{
    stream << "[" << card.first.getId() << "] ";
    stream << card.first.getName() << " (" << card.second << ")";
    stream << "\n";
}
