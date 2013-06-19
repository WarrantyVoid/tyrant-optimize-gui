#include "CTyrantOptimizeWrapper.h"
#include "CGlobalConfig.h"
#include <QStringList>

const QString CTyrantOptimizeWrapper::PROCESS_NAME = "tyrant_optimize.exe";

CTyrantOptimizeWrapper::CTyrantOptimizeWrapper()
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
    if (guiParams.anpOnly())
    {
        comLineParams << "-a";
    }
    if (guiParams.lockCardCount())
    {
    //    comLineParams << "-fixedlen";
        if (guiParams.lockCardCountMin() > 1)
        {
            comLineParams << "minlen" << QString("%1").arg(guiParams.lockCardCountMin());
        }
        if (guiParams.lockCardCountMax() < 10)
        {
            comLineParams << "maxlen" << QString("%1").arg(guiParams.lockCardCountMax());
        }
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
    if (guiParams.winTie())
    {
        comLineParams << "-wintie";
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
}

void CTyrantOptimizeWrapper::processCommandLineOutput(const QStringList &output)
{
    for (int iLine = 0; iLine < output.size(); ++iLine)
    {
        QString curLine = output.at(iLine);
        if (curLine.startsWith("Attacker:"))
        {
            emit deckUpdated("");
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:"), QString::SkipEmptyParts);
            if (!curTokens.isEmpty())
            {
                emit deckUpdated(curTokens.last());
            }
        }
        else if (curLine.startsWith("win%"))
        {
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:"), QString::SkipEmptyParts);
            bool ok(true);
            float winPercent = curTokens.at(1).toFloat(&ok);
            if (ok)
            {
                emit winChanceUpdated(winPercent);
            }
        }
        else if (curLine.startsWith("ANP"))
        {
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:"), QString::SkipEmptyParts);
            bool ok(true);
            float anp = curTokens.at(1).toFloat(&ok);
            if (ok)
            {
                emit anpUpdated(anp);
            }
        }
        else if (curLine.startsWith("Deck improved"))
        {
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:"), QString::SkipEmptyParts);

            //Debug output, enable only for testing
            //QString debugOut = "";
            //for (int iToken = 0; iToken < curTokens.size(); ++iToken)
            //{
            //    debugOut += "[" + curTokens.at(iToken) + "]";
            //}
            //mProcessStatusLabel->setText(debugOut);
            //return;

            if (curTokens.size() > 1)
            {
                emit deckUpdated(curTokens.at(2));
            }
        }
        else
        {
            // Extra output starting with win chance
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:|%"), QString::SkipEmptyParts);
            bool ok(true);
            float winPercent = curTokens.at(0).toFloat(&ok);
            if (ok)
            {
                int chanceIndex = curLine.indexOf('%');
                if (chanceIndex > -1 && chanceIndex < 8)
                {
                    emit winChanceUpdated(winPercent);
                }
                else
                {
                    emit anpUpdated(winPercent);
                }
            }
        }
    }
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
