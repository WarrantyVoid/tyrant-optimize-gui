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
    int mMinAttack;
    int mMinHp;
};

#endif // CCARDSEARCHPARAMETERS_H
