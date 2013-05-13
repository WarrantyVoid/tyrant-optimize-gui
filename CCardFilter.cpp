#include "CCardFilter.h"
#include <QFile>

CCardFilter::CCardFilter()
: mCards(CCardTable::getCardTable())
{
}

SOwnedCardStatistics CCardFilter::writeOwnedCardsToFile(const QString &filePath, const QList<TOwnedCard> &cards)
{
    SOwnedCardStatistics stats;
    stats.numCards = 0;
    stats.numCopies = 0;
    if (!cards.isEmpty())
    {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            QTextStream outStream(&file);
            for (QList<TOwnedCard>::const_iterator i = cards.begin(); i != cards.end(); ++i)
            {
                writeOwnedCard(outStream, *i);
                ++stats.numCards;
                stats.numCopies += (*i).second;
            }
        }
    }
    return stats;
}

SOwnedCardStatistics CCardFilter::readOwnedCardsFromFile(const QString &filePath, QList<TOwnedCard> &cards)
{
    SOwnedCardStatistics stats;
    stats.numCards = 0;
    stats.numCopies = 0;
    cards.clear();
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        while (!file.atEnd())
        {
            QString curCardStr(file.readLine());
            TOwnedCard curOwned;
            if (readOwnedCard(curCardStr, curOwned))
            {
                cards.push_back(curOwned);
                ++stats.numCards;
                stats.numCopies += curOwned.second;
            }
        }
    }
    return stats;
}

SOwnedCardStatistics CCardFilter::readOwnedCardsFromClipboard(const QString &clipboardStr, QList<TOwnedCard> &cards)
{
    SOwnedCardStatistics stats;
    stats.numCards = 0;
    stats.numCopies = 0;
    cards.clear();
    if (!clipboardStr.isEmpty() && clipboardStr.length() < 1024 * 1024)
    {
        QStringList cardsSplit = clipboardStr.split("\n");
        for (QStringList::const_iterator i = cardsSplit.begin(); i != cardsSplit.end(); ++i)
        {
            const QString &curCardStr = *i;
            TOwnedCard curOwned;
            if (readOwnedCard(curCardStr, curOwned))
            {
                cards.push_back(curOwned);
                ++stats.numCards;
                stats.numCopies += curOwned.second;
            }
        }
    }
    return stats;
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
