#ifndef CPROCESSPARAMETERS_H
#define CPROCESSPARAMETERS_H

#include <QString>
#include <QSettings>

namespace Ui
{
class MainWindow;
}

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

	QString baseDeck() const { return mBaseDeck; }
	QString enemyDeck() const { return mEnemyDeck; }
	QString battleGround() const { return mBattleGround; }
    QString ownedCardsFile() const { return mOwnedCardsFile; }
    unsigned int achievement() const { return mAchievement; }
	int numThreads() const { return mNumThreads; }
	int numTurns() const { return mNumTurns; }
	int numBattles() const { return mNumBattles; }
	bool anpOnly() const { return mAnpOnly; }
	bool lockCommander() const { return mLockCommander; }
    bool lockCardCount() const { return mLockCardCount; }
	bool ownedCardsOnly() const { return mOwnedCardsOnly; }    
    bool orderedBase() const { return mOrderedBase; }
    bool orderedEnemy() const { return mOrderedEnemy; }
    bool surge() const { return mSurge; }
    bool winTie() const { return mWinTie; }
    bool tournament() const { return mTournament; }
    
private:
    bool mIsOptimizationEnabled;
    QString mBaseDeck;
	QString mEnemyDeck;
	QString mBattleGround;
    QString mOwnedCardsFile;
    unsigned int mAchievement;
	int mNumThreads;
	int mNumTurns;
	int mNumBattles;
	bool mAnpOnly;
	bool mLockCommander;
    bool mLockCardCount;
	bool mOwnedCardsOnly;
    bool mOrderedBase;
    bool mOrderedEnemy;
    bool mSurge;
    bool mWinTie;
    bool mTournament;
};

#endif // CPROCESSPARAMETERS_H
