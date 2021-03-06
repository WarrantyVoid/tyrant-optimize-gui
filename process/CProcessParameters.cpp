#include "CProcessParameters.h"
#include "ui_MainWindow.h"
#include "model/CCardTable.h"
#include <QLineEdit>

CProcessParameters::CProcessParameters()
: mProcessMode(EProcessOptimize)
, mBaseDeckOrig("")
, mBaseDeckOut("")
, mEnemyDeck("")
, mBattleGround("")
, mOwnedCardsFile("ownedcards.txt")
, mAchievement(0u)
, mLockCardCountMin(1u)
, mLockCardCountMax(10u)
, mNumThreads(4)
, mNumTurns(50)
, mNumBattles(10000)
, mPriority(ENormalPriority)
, mOptimizationMode(EOptimizeWin)
, mLockCardCount(false)
, mOwnedCardsOnly(false)
, mOrderedBase(false)
, mOrderedEnemy(false)
, mSurge(false)
, mTournament(false)
{
}

void CProcessParameters::fetchFromUi(const Ui::MainWindow &ui)
{
    mBaseDeckOrig = ui.baseDeckEdit->currentText().trimmed();
    mBaseDeckOut.clear();
    CCardTable &cardTable = CCardTable::getCardTable();
    const QList<CCard> &cards = ui.baseDeckWidget->getDeck().getCards();

    // Preprocess provided deck
    TCardStatusMap deckStatus;
    for (int i = 0; i < cards.size(); ++i)
    {
        // Apply card filters
        SCardStatus status;
        const CCard *card = &cards[i];
        if (ui.ownedCardsBox->isChecked())
        {
            for (int iTry = 0; iTry < 2 && status.numOwnedFiltered <= 0; ++iTry)
            {
                if (iTry > 0)
                {
                    card = &cardTable.getOwnedCardEquivalent(cards[i], deckStatus);
                }

                TCardStatusMap::iterator iEx = deckStatus.find(card->getId());
                if (iEx != deckStatus.end())
                {
                    status = iEx.value();
                    --iEx.value().numOwnedFiltered;
                }
                else
                {
                    status = cardTable.getCardStatus(*card);
                    SCardStatus newStatus = status;
                    --newStatus.numOwnedFiltered;
                    deckStatus.insert(card->getId(), newStatus);
                }

                // Ignore ownage / blacklist status
                if (ui.baseDeckWidget->isLocked(i - 1))
                {
                    break;
                }
            }
        }
        else
        {
            status.numOwned = 10;
            status.numOwnedFiltered = 10;
        }
        bool countLockCheck = (!ui.cardCountLockBox->isChecked() ||  i <= ui.maxCardCountLockBox->value());
        if (card && card->isValid() && countLockCheck)
        {
            // Apply card locks
            mBaseDeckOut.append(QString("%1%2[%3]%4")
                .arg((i > 0) ? "," : "")
                .arg(card == &cards[i] && ui.baseDeckWidget->isLocked(i - 1) ? "!" : "")
                .arg(card->getId())
                .arg(card->getName()));
        }
    }

    mEnemyDeck = ui.enemyDeckEdit->currentText().trimmed();
    mBattleGround = ui.battleGroundBox->currentText();
    mOwnedCardsFile = ui.ownedCardsFileBox->currentText();
    mAchievement = ui.achievementBox->itemData(ui.achievementBox->currentIndex()).toUInt();
    mLockCardCountMin = ui.minCardCountLockBox->value();
    mLockCardCountMax = ui.maxCardCountLockBox->value();
	mNumThreads = ui.numThreadsSpinBox->value();
	mNumTurns = ui.numTurnsSpinBox->value();
	mNumBattles = ui.numBattlesSpinBox->value();
    mPriority = static_cast<EProcessPriority>(ui.priorityBox->currentIndex());
    mOptimizationMode = static_cast<EOptimizationMode>(ui.optimizationModeBox->currentIndex());
    mLockCardCount = ui.cardCountLockBox->isChecked();
	mOwnedCardsOnly = ui.ownedCardsBox->isChecked();
    mOrderedBase = ui.orderedBaseBox->isChecked();
    mOrderedEnemy = ui.orderedEnemyBox->isChecked();
	mSurge = ui.surgeBox->isChecked();
    mTournament = ui.tournamentBox->isChecked();
}

void CProcessParameters::updateUi(Ui::MainWindow &ui) const
{
    ui.baseDeckEdit->setDeckId(mBaseDeckOrig);

    ui.baseDeckWidget->setDeck(mBaseDeckOrig);
    QStringList deck = mBaseDeckOut.split(",");
    for (int i = 0; i < deck.size(); ++i)
    {
        ui.baseDeckWidget->setLocked(i - 1, deck[i].startsWith("!"));
    }

    ui.enemyDeckEdit->setDeckId(mEnemyDeck);
    ui.enemyDeckWidget->setDeck(mEnemyDeck);
    ui.battleGroundBox->setCurrentIndex(ui.battleGroundBox->findText(mBattleGround)); 
    ui.ownedCardsFileBox->setCurrentIndex(qMax(0, ui.ownedCardsFileBox->findText(mOwnedCardsFile)));
    ui.achievementBox->setCurrentIndex(ui.achievementBox->findData(mAchievement));
    ui.minCardCountLockBox->setValue(mLockCardCountMin);
    ui.maxCardCountLockBox->setValue(mLockCardCountMax);
	ui.numThreadsSpinBox->setValue(mNumThreads);
	ui.numTurnsSpinBox->setValue(mNumTurns);
	ui.numBattlesSpinBox->setValue(mNumBattles);
    ui.priorityBox->setCurrentIndex(static_cast<int>(mPriority));
    ui.optimizationModeBox->setCurrentIndex(static_cast<int>(mOptimizationMode));
    ui.cardCountLockBox->setChecked(mLockCardCount);
	ui.ownedCardsBox->setChecked(mOwnedCardsOnly);
    ui.orderedBaseBox->setChecked(mOrderedBase);
    ui.orderedEnemyBox->setChecked(mOrderedEnemy);
	ui.surgeBox->setChecked(mSurge);
    ui.tournamentBox->setChecked(mTournament);
}

void CProcessParameters::fetchFromSettings(QSettings &settings)
{
	settings.beginGroup("parameters");
    mBaseDeckOrig = settings.value("baseDeck", mBaseDeckOrig).toString();
    mBaseDeckOut = settings.value("baseDeckOut", mBaseDeckOut).toString();
	mEnemyDeck = settings.value("enemyDeck", mEnemyDeck).toString();
	mBattleGround = settings.value("battleGround", mBattleGround).toString();
    mOwnedCardsFile = settings.value("ownedCardsFile", mOwnedCardsFile).toString();
    mAchievement = settings.value("achievement", mAchievement).toUInt();
    mLockCardCountMin = settings.value("lockCardCountMin", mLockCardCountMin).toUInt();
    mLockCardCountMax = settings.value("lockCardCountMax", mLockCardCountMax).toUInt();
	mNumThreads = settings.value("numThreads", mNumThreads).toInt();
	mNumTurns = settings.value("numTurns", mNumTurns).toInt();
	mNumBattles = settings.value("numBattles", mNumBattles).toInt();
    mPriority = static_cast<EProcessPriority>(settings.value("priority", static_cast<int>(mPriority)).toInt());
    mOptimizationMode = static_cast<EOptimizationMode>(settings.value("optimizationMode", static_cast<int>(mOptimizationMode)).toInt());
    mLockCardCount = settings.value("lockCardCount", mLockCardCount).toBool();
	mOwnedCardsOnly = settings.value("ownedCardsOnly", mOwnedCardsOnly).toBool();
    mOrderedBase = settings.value("orderedBase", mOrderedBase).toBool();
    mOrderedEnemy = settings.value("orderedEnemy", mOrderedEnemy).toBool();
	mSurge = settings.value("surge", mSurge).toBool();
    mTournament = settings.value("tournament", mTournament).toBool();
	settings.endGroup();
}

 void CProcessParameters::updateSettings(QSettings &settings) const
 {
	settings.beginGroup("parameters");
    settings.setValue("baseDeck", mBaseDeckOrig);
    settings.setValue("baseDeckOut", mBaseDeckOut);
	settings.setValue("enemyDeck", mEnemyDeck);
	settings.setValue("battleGround", mBattleGround);
    settings.setValue("ownedCardsFile", mOwnedCardsFile);
    settings.setValue("achievement", mAchievement);
    settings.setValue("lockCardCountMin", mLockCardCountMin);
    settings.setValue("lockCardCountMax", mLockCardCountMax);
	settings.setValue("numThreads", mNumThreads);
	settings.setValue("numTurns", mNumTurns);
	settings.setValue("numBattles", mNumBattles);
    settings.setValue("priority", static_cast<int>(mPriority));
    settings.setValue("optimizationMode", static_cast<int>(mOptimizationMode));
    settings.setValue("lockCardCount", mLockCardCount);
	settings.setValue("ownedCardsOnly", mOwnedCardsOnly);
    settings.setValue("orderedBase", mOrderedBase);
    settings.setValue("orderedEnemy", mOrderedEnemy);
	settings.setValue("surge", mSurge);
    settings.setValue("tournament", mTournament);
	settings.endGroup();
 }

