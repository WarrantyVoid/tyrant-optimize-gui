#ifndef CPROCESSPARAMETERS_H
#define CPROCESSPARAMETERS_H

#include <QString>
#include <QSettings>
#include "CProcessPriority.h"

namespace Ui
{
class MainWindow;
}


enum EOptimizationMode
{
    EOptimizeWin = 0,
    EOptimizeDefense = 1,
    EOptimizeRaid = 2
};

class CProcessParameters
{

public:
	CProcessParameters();

public:
    void fetchFromUi(const Ui::MainWindow &ui);
	void updateUi(Ui::MainWindow &ui) const;

	void fetchFromSettings(QSettings &settings);
	void updateSettings(QSettings &settings) const;

    void setOptimizationEnabled(bool enabled) { mIsOptimizationEnabled = enabled; }
    bool isOptimizationEnabled() const { return mIsOptimizationEnabled; }

    QString baseDeck() const { return mBaseDeckOut; }
	QString enemyDeck() const { return mEnemyDeck; }
	QString battleGround() const { return mBattleGround; }
    QString ownedCardsFile() const { return mOwnedCardsFile; }
    unsigned int achievement() const { return mAchievement; }
    unsigned int lockCardCountMin() const { return mLockCardCountMin; }
    unsigned int lockCardCountMax() const { return mLockCardCountMax; }
	int numThreads() const { return mNumThreads; }
	int numTurns() const { return mNumTurns; }
	int numBattles() const { return mNumBattles; }
    EProcessPriority priority() const { return mPriority; }
    EOptimizationMode optimizationMode() const { return mOptimizationMode; }
    bool lockCardCount() const { return mLockCardCount; }
	bool ownedCardsOnly() const { return mOwnedCardsOnly; }    
    bool orderedBase() const { return mOrderedBase; }
    bool orderedEnemy() const { return mOrderedEnemy; }
    bool surge() const { return mSurge; }
    bool tournament() const { return mTournament; }
    
private:
    bool mIsOptimizationEnabled;
    QString mBaseDeckOrig;
    QString mBaseDeckOut;
	QString mEnemyDeck;
	QString mBattleGround;
    QString mOwnedCardsFile;
    unsigned int mAchievement;
    unsigned int mLockCardCountMin;
    unsigned int mLockCardCountMax;
	int mNumThreads;
	int mNumTurns;
	int mNumBattles;
    EProcessPriority mPriority;
    EOptimizationMode mOptimizationMode;
    bool mLockCardCount;
	bool mOwnedCardsOnly;
    bool mOrderedBase;
    bool mOrderedEnemy;
    bool mSurge;
    bool mTournament;
};

#endif // CPROCESSPARAMETERS_H
