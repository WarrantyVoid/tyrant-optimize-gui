#include "CTyrantOptimizeWrapper.h"
#include "CGlobalConfig.h"
#include <QStringList>

const QString CTyrantOptimizeWrapper::PROCESS_NAME = "tyrant_optimize";

CTyrantOptimizeWrapper::CTyrantOptimizeWrapper()
: mStatus()
{
}

CTyrantOptimizeWrapper::~CTyrantOptimizeWrapper()
{

}

QFileInfo CTyrantOptimizeWrapper::getProcessExecutable()
{
    const CGlobalConfig &cfg = CGlobalConfig::getCfg();
    return QFileInfo(cfg.getToolPath() + PROCESS_NAME);
}

void CTyrantOptimizeWrapper::getCommandLineParameters(const CProcessParameters &guiParams, QStringList &comLineParams)
{
    const CGlobalConfig &cfg = CGlobalConfig::getCfg();
    comLineParams.clear();

    // Decks
    comLineParams << processDeckString(guiParams.baseDeck());
    comLineParams << processDeckString(guiParams.enemyDeck());

    // Switches
    if (guiParams.lockCardCount())
    {
        comLineParams << "-L"
                      << QString("%1").arg(guiParams.lockCardCountMin())
                      << QString("%1").arg(guiParams.lockCardCountMax());
    }

    if (guiParams.ownedCardsOnly())
    {
        if (QFileInfo(cfg.getToolPath() + "ownedcards_f.txt").exists())
        {
            comLineParams << "-o=ownedcards_f.txt";
        }
        else
        {
            comLineParams << "-o";
        }
    }
    if (guiParams.orderedBase())
    {
        comLineParams << "-r";
    }
    if (guiParams.orderedEnemy())
    {
        comLineParams << "defender:ordered";
    }
    if (guiParams.surge())
    {
        comLineParams << "-s";
    }
    if (guiParams.tournament())
    {
        comLineParams << "tournament";
    }
    if (!guiParams.battleGround().isEmpty())
    {
        comLineParams << "-e" << guiParams.battleGround();
    }
    if (guiParams.achievement() > 0)
    {
        comLineParams << "-A" << QString("%1").arg(guiParams.achievement());
    }

    if (guiParams.isOptimizationEnabled())
    {
        comLineParams << "climb" << QString("%1").arg(guiParams.numBattles());
    }
    else
    {
        comLineParams << "sim" << QString("%1").arg(guiParams.numBattles());
    }
    comLineParams << "-t" << QString("%1").arg(guiParams.numThreads());

    if (guiParams.numTurns() != 50)
    {
        comLineParams << "-turnlimit" << QString("%1").arg(guiParams.numTurns());
    }

    switch (guiParams.optimizationMode())
    {
    case EOptimizeWin: comLineParams << "win"; break;
    case EOptimizeDefense: comLineParams << "defense"; break;
    case EOptimizeRaid: comLineParams << "raid"; break;
    }
}

void CTyrantOptimizeWrapper::processCommandLineOutput(const QStringList &output)
{
    for (int iLine = 0; iLine < output.size(); ++iLine)
    {
        QString curLine = output.at(iLine);
        if (curLine.startsWith("Your Deck:"))
        {
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:"), QString::SkipEmptyParts);
            if (!curTokens.isEmpty())
            {
                mStatus.deckHash = curTokens.last();
            }
        }
        else if (curLine.startsWith("win%") || curLine.startsWith("kill%"))
        {
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:"), QString::SkipEmptyParts);
            bool ok(true);
            float winPercent = curTokens.at(1).toFloat(&ok);
            if (ok)
            {
                mStatus.chanceWin = winPercent;
            }
        }
        else if (curLine.startsWith("stall%"))
        {
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:"), QString::SkipEmptyParts);
            bool ok(true);
            float stallPercent = curTokens.at(1).toFloat(&ok);
            if (ok)
            {
                mStatus.chanceStall = stallPercent;
            }
        }
        else if (curLine.startsWith("loss%"))
        {
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:"), QString::SkipEmptyParts);
            bool ok(true);
            float lossPercent = curTokens.at(1).toFloat(&ok);
            if (ok)
            {
                mStatus.chanceLoss = lossPercent;
            }
        }
        else if (curLine.startsWith("ard"))
        {
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:"), QString::SkipEmptyParts);
            bool ok(true);
            float ard = curTokens.at(1).toFloat(&ok);
            if (ok)
            {
                mStatus.avRaidDmg = ard;
            }
        }
        else if (curLine.startsWith("Deck improved"))
        {
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:"), QString::SkipEmptyParts);
            if (curTokens.size() > 1)
            {
                mStatus.deckHash = curTokens.at(2);
            }
        }
        else
        {
            // Extra output starting with win chance
            QStringList curTokens = curLine.split(QRegExp("\\s|\\,|\\(|\\)|:|%"), QString::SkipEmptyParts);
            bool killFound(false);
            bool stallFound(false);
            bool otherFound(false);
            bool ok(true);
            for (int idx = 0; idx < 5; idx += 2)
            {
                if (idx + 1 < curTokens.size())
                {
                    const QString& typeToken = curTokens.at(idx + 1);
                    if (typeToken.compare("kill") == 0)
                    {
                        float winPercent = curTokens.at(idx).toFloat(&ok);
                        if (ok)
                        {
                            mStatus.chanceWin = winPercent;
                            killFound = true;
                        }
                    }
                    else if (typeToken.compare("stall") == 0)
                    {
                        float stallPercent = curTokens.at(idx).toFloat(&ok);
                        if (ok)
                        {
                            mStatus.chanceStall = stallPercent;
                            stallFound = true;
                        }
                    }
                    else
                    {
                        float otherValue = curTokens.at(idx).toFloat(&ok);
                        if (ok)
                        {
                            if (idx == 4)
                            {
                                mStatus.avRaidDmg = otherValue;
                            }
                            else
                            {
                                mStatus.chanceWin = otherValue * 100.0f;
                            }
                            otherFound = true;
                        }
                    }
                }
            }
            if (killFound || stallFound || otherFound)
            {
                if (stallFound && otherFound && !killFound)
                {
                    // Stall chance is included when win% is other
                    mStatus.chanceWin -= mStatus.chanceStall;
                }
                mStatus.chanceLoss = 100.0f - mStatus.chanceWin - mStatus.chanceStall;
                emit statusUpdated(mStatus);
            }
        }
    }
}

void CTyrantOptimizeWrapper::processInit()
{
    mStatus = SOptimizationStatus();
}

QString CTyrantOptimizeWrapper::processDeckString(const QString &deckStr)
{
    QStringList subDecks = deckStr.split(";", QString::SkipEmptyParts);
    for (QStringList::iterator i = subDecks.begin(); i != subDecks.end(); ++i)
    {
        QString &curStr = *i;
        curStr = curStr.trimmed().remove("\"");
        if (curStr.startsWith("Mission", Qt::CaseInsensitive))
        {
            curStr = QString("Mission ") + curStr.mid(7).trimmed();
        }
        if (curStr.startsWith("Step", Qt::CaseInsensitive))
        {
            curStr = QString("Step ") + curStr.mid(4).trimmed();
        }
    }
    return subDecks.join(";");
}

void CTyrantOptimizeWrapper::processFinished()
{
    emit statusUpdated(mStatus);
}
