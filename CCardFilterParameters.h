#ifndef CCARDFILTERPARAMETERS_H
#define CCARDFILTERPARAMETERS_H

#include <QHash>
#include <QSettings>
#include "ICardCheck.h"

namespace Ui
{
class CardFilterWidget;
}
class CCardTable;
class CCard;

class CCardFilterParameters : public ICardCheck
{
public:
    CCardFilterParameters();

public:
    void fetchFromUi(const Ui::CardFilterWidget &ui);
    void updateUi(Ui::CardFilterWidget &ui) const;

    void fetchFromSettings(QSettings &settings);
    void updateSettings(QSettings &settings) const;

    bool checkCard(const CCard &card, int &num) const;

private:
    static const int NUM_RARITY = 4;
    static const int NUM_HEALTH = 5;
    static const int NUM_ATTACK = 5;
    static const int NUM_DELAY = 5;
    static const int NUM_FACTION = 5;
    static const int NUM_SET = 16;

private:
    CCardTable &mCards;
    bool mIsAssaultAllowed[NUM_RARITY];
    bool mIsStructureAllowed[NUM_RARITY];
    bool mIsCommanderAllowed[NUM_RARITY];
    bool mIsActionAllowed[NUM_RARITY];
    bool mIsHealthAllowed[NUM_HEALTH];
    bool mIsAttackAllowed[NUM_ATTACK];
    bool mIsDelayAllowed[NUM_DELAY];
    bool mIsFactionAllowed[NUM_FACTION];
    int mComplement[NUM_SET];
    QHash<QString, int> mWhiteList;
    QHash<QString, int> mBlackList;
    bool mIsFilterOptionEnabled;
    bool mIsWhiteListEnabled;
    bool mIsBlackListEnabled;
    bool mIsCompletionEnabled;
};

#endif // CCARDFILTERPARAMETERS_H
