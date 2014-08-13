#ifndef CCARDSEARCHPARAMETERS_H
#define CCARDSEARCHPARAMETERS_H

#include <QStringList>
#include <QSettings>
#include "ICardCheck.h"

namespace Ui
{
class CardSearchWidget;
}
class CCardTable;

enum EComparisonMethod
{
    ECompareLarger,
    ECompareEqualOrLarger,
    ECompareEqual,
    ECompareEqualOrLesser,
    ECompareLesser
};

class CCardSearchParameters : public ICardCheck
{
public:
    CCardSearchParameters();

public:
    void fetchFromUi(const Ui::CardSearchWidget &ui);
    void updateUi(Ui::CardSearchWidget &ui) const;

    void fetchFromSettings(QSettings &settings);
    void updateSettings(QSettings &settings) const;

    bool checkCard(const CCard &card, int &num) const;

private:
    CCardTable &mCards;
    QString mName;
    QString mLwCsName;
    QStringList mSkill;
    QStringList mLwCsSkill;
    typedef int TSearchMask;
    TSearchMask mRarityMask;
    TSearchMask mTypeMask;
    TSearchMask mFactionMask;
    TSearchMask mTimerMask;
    EComparisonMethod mAttackCompare;
    EComparisonMethod mHpCompare;
    int mAttackValue;
    int mHpValue;
    int mUpgradeLevel;
    bool mCheckUpgradeLevel;
    bool mIsUnique;

};

#endif // CCARDSEARCHPARAMETERS_H
