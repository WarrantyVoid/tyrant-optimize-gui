#include "CProcessParameters.h"
#include "ui_MainWindow.h"
#include <QLineEdit>

CProcessParameters::CProcessParameters()
: mIsOptimizationEnabled(true)
, mBaseDeck("")
, mEnemyDeck("")
, mBattleGround("")
, mAchievement("")
, mOwnedCardsFile("ownedcards.txt")
, mNumThreads(4)
, mNumTurns(50)
, mNumBattles(10000)
, mAnpOnly(false)
, mLockCommander(false)
, mLockCardCount(false)
, mOwnedCardsOnly(false)
, mOrderedBase(false)
, mOrderedEnemy(false)
, mSurge(false)
, mWinTie(false)
, mTournament(false)
{
}

void CProcessParameters::fetchFromUi(const Ui::MainWindow &ui)
{    
    mBaseDeck = ui.baseDeckEdit->currentText().trimmed();
    mEnemyDeck = ui.enemyDeckEdit->currentText().trimmed();
	mBattleGround = ui.battleGroundBox->currentText();   
    mAchievement = ui.achievementBox->currentText();
    mOwnedCardsFile = ui.ownedCardsFileBox->currentText();
	mNumThreads = ui.numThreadsSpinBox->value();
	mNumTurns = ui.numTurnsSpinBox->value();
	mNumBattles = ui.numBattlesSpinBox->value();
	mAnpOnly = ui.anpOnlyBox->isChecked();
	mLockCommander = ui.commanderLockBox->isChecked();
    mLockCardCount = ui.cardCountLockBox->isChecked();
	mOwnedCardsOnly = ui.ownedCardsBox->isChecked();
    mOrderedBase = ui.orderedBaseBox->isChecked();
    mOrderedEnemy = ui.orderedEnemyBox->isChecked();
	mSurge = ui.surgeBox->isChecked();
    mWinTie = ui.winTieBox->isChecked();
    mTournament = ui.tournamentBox->isChecked();
}

void CProcessParameters::updateUi(Ui::MainWindow &ui) const
{
    ui.baseDeckEdit->lineEdit()->setText(mBaseDeck);
    ui.enemyDeckEdit->lineEdit()->setText(mEnemyDeck);
    ui.battleGroundBox->setCurrentIndex(ui.battleGroundBox->findText(mBattleGround));
    ui.achievementBox->setCurrentIndex(ui.achievementBox->findText(mAchievement));
    ui.ownedCardsFileBox->setCurrentIndex(ui.ownedCardsFileBox->findText(mOwnedCardsFile));
	ui.numThreadsSpinBox->setValue(mNumThreads);
	ui.numTurnsSpinBox->setValue(mNumTurns);
	ui.numBattlesSpinBox->setValue(mNumBattles);
	ui.anpOnlyBox->setChecked(mAnpOnly);
	ui.commanderLockBox->setChecked(mLockCommander);
    ui.cardCountLockBox->setChecked(mLockCardCount);
	ui.ownedCardsBox->setChecked(mOwnedCardsOnly);
    ui.orderedBaseBox->setChecked(mOrderedBase);
    ui.orderedEnemyBox->setChecked(mOrderedEnemy);
	ui.surgeBox->setChecked(mSurge);
    ui.winTieBox->setChecked(mWinTie);
    ui.tournamentBox->setChecked(mTournament);
}

void CProcessParameters::fetchFromSettings(QSettings &settings)
{
	settings.beginGroup("parameters");
	mBaseDeck = settings.value("baseDeck", mBaseDeck).toString();
	mEnemyDeck = settings.value("enemyDeck", mEnemyDeck).toString();
	mBattleGround = settings.value("battleGround", mBattleGround).toString();
    mAchievement = settings.value("achievement", mAchievement).toString();
    mOwnedCardsFile = settings.value("ownedCardsFile", mOwnedCardsFile).toString();
	mNumThreads = settings.value("numThreads", mNumThreads).toInt();
	mNumTurns = settings.value("numTurns", mNumTurns).toInt();
	mNumBattles = settings.value("numBattles", mNumBattles).toInt();
	mAnpOnly = settings.value("anpOnly",mAnpOnly).toBool();
	mLockCommander = settings.value("lockCommander", mLockCommander).toBool();
    mLockCardCount = settings.value("lockCardCount", mLockCardCount).toBool();
	mOwnedCardsOnly = settings.value("ownedCardsOnly", mOwnedCardsOnly).toBool();
    mOrderedBase = settings.value("orderedBase", mOrderedBase).toBool();
    mOrderedEnemy = settings.value("orderedEnemy", mOrderedEnemy).toBool();
	mSurge = settings.value("surge", mSurge).toBool();
    mWinTie = settings.value("winTie", mWinTie).toBool();
    mTournament = settings.value("tournament", mTournament).toBool();
	settings.endGroup();
}

 void CProcessParameters::updateSettings(QSettings &settings) const
 {
	settings.beginGroup("parameters");
	settings.setValue("baseDeck", mBaseDeck);
	settings.setValue("enemyDeck", mEnemyDeck);
	settings.setValue("battleGround", mBattleGround);
    settings.setValue("achievement", mAchievement);
    settings.setValue("ownedCardsFile", mOwnedCardsFile);
	settings.setValue("numThreads", mNumThreads);
	settings.setValue("numTurns", mNumTurns);
	settings.setValue("numBattles", mNumBattles);
	settings.setValue("anpOnly", mAnpOnly);
	settings.setValue("lockCommander", mLockCommander);
    settings.setValue("lockCardCount", mLockCardCount);
	settings.setValue("ownedCardsOnly", mOwnedCardsOnly);
    settings.setValue("orderedBase", mOrderedBase);
    settings.setValue("orderedEnemy", mOrderedEnemy);
	settings.setValue("surge", mSurge);
    settings.setValue("winTie", mWinTie);
    settings.setValue("tournament", mTournament);
	settings.endGroup();
 }

