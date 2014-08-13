#include "CCardSearchParameters.h"
#include "ui_CardSearchWidget.h"
#include "model/CCard.h"
#include "model/CSkill.h"
#include "model/CCardTable.h"
#include <QLineEdit>

CCardSearchParameters::CCardSearchParameters()
: mCards(CCardTable::getCardTable())
, mName()
, mLwCsName()
, mSkill()
, mLwCsSkill()
, mRarityMask(0)
, mTypeMask(0)
, mFactionMask(0)
, mTimerMask(0)
, mAttackCompare(ECompareEqualOrLarger)
, mHpCompare(ECompareEqualOrLarger)
, mAttackValue(0)
, mHpValue(0)
, mUpgradeLevel(0xff)
, mCheckUpgradeLevel(false)
, mIsUnique(false)
{
}

void CCardSearchParameters::fetchFromUi(const Ui::CardSearchWidget &ui)
{
    mName = ui.nameBox->currentText().trimmed();
    mLwCsName = mName.toLower();
    mSkill = ui.skillBox->currentText().split("+", QString::SkipEmptyParts);
    mLwCsSkill.clear();
    for (QStringList::const_iterator i = mSkill.begin(); i != mSkill.end(); ++i)
    {
        mLwCsSkill.push_back(i->toLower().trimmed());
    }

    mRarityMask = 0;
    mRarityMask |= ui.legendaryButton->isChecked() ? 0x1 : 0;
    mRarityMask |= ui.rareButton->isChecked() ? 0x2 : 0;
    mRarityMask |= ui.uncommonButton->isChecked() ? 0x4 : 0;
    mRarityMask |= ui.commonButton->isChecked() ? 0x8 : 0;

    mTypeMask = 0;
    mTypeMask |= ui.assaultButton->isChecked() ? 0x1 : 0;
    mTypeMask |= ui.structureButton->isChecked() ? 0x2 : 0;
    mTypeMask |= ui.commanderButton->isChecked() ? 0x4 : 0;
    mTypeMask |= ui.actionButton->isChecked() ? 0x8 : 0;

    mFactionMask = 0;
    mFactionMask |= ui.bloodthirstyButton->isChecked() ? 0x1 : 0;
    mFactionMask |= ui.imperialButton->isChecked() ? 0x2 : 0;
    mFactionMask |= ui.raiderButton->isChecked() ? 0x4 : 0;
    mFactionMask |= ui.righteousButton->isChecked() ? 0x8 : 0;
    mFactionMask |= ui.xenoButton->isChecked() ? 0x10 : 0;

    mTimerMask = 0;
    mTimerMask |= ui.timer0Button->isChecked() ? 0x1 : 0;
    mTimerMask |= ui.timer1Button->isChecked() ? 0x2 : 0;
    mTimerMask |= ui.timer2Button->isChecked() ? 0x4 : 0;
    mTimerMask |= ui.timer3Button->isChecked() ? 0x8 : 0;
    mTimerMask |= ui.timer4Button->isChecked() ? 0x10 : 0;

    switch(ui.attackStackedWidget->currentIndex())
    {
    case 1: mAttackCompare = ECompareEqual; break;
    case 2: mAttackCompare = ECompareEqualOrLesser; break;
    default: mAttackCompare = ECompareEqualOrLarger; break;
    }
    switch(ui.hpStackedWidget->currentIndex())
    {
    case 1: mHpCompare = ECompareEqual; break;
    case 2: mHpCompare = ECompareEqualOrLesser; break;
    default: mHpCompare = ECompareEqualOrLarger; break;
    }

    mAttackValue = ui.attackSlider->value();
    mHpValue = ui.hpSlider->value();

    mCheckUpgradeLevel = true;
    switch(ui.upgradeStackedWidget->currentIndex())
    {
    case 1: mUpgradeLevel = 0; break;
    case 2: mUpgradeLevel = 1; break;
    case 3: mUpgradeLevel = 2; break;
    default: mUpgradeLevel = 0xff; mCheckUpgradeLevel = false; break;
    }
    mIsUnique = ui.uniqueButton->isChecked();
}

void CCardSearchParameters::updateUi(Ui::CardSearchWidget &ui) const
{
    ui.nameBox->lineEdit()->setText(mName);
    ui.skillBox->lineEdit()->setText(mSkill.join("+"));

    ui.legendaryButton->setChecked((mRarityMask & 0x1) != 0);
    ui.rareButton->setChecked((mRarityMask & 0x2) != 0);
    ui.uncommonButton->setChecked((mRarityMask & 0x4) != 0);
    ui.commonButton->setChecked((mRarityMask & 0x8) != 0);

    ui.assaultButton->setChecked((mTypeMask & 0x1) != 0);
    ui.structureButton->setChecked((mTypeMask & 0x2) != 0);
    ui.commanderButton->setChecked((mTypeMask & 0x4) != 0);
    ui.actionButton->setChecked((mTypeMask & 0x8) != 0);

    ui.bloodthirstyButton->setChecked((mFactionMask & 0x1) != 0);
    ui.imperialButton->setChecked((mFactionMask & 0x2) != 0);
    ui.raiderButton->setChecked((mFactionMask & 0x4) != 0);
    ui.righteousButton->setChecked((mFactionMask & 0x8) != 0);
    ui.xenoButton->setChecked((mFactionMask & 0x10) != 0);

    ui.timer0Button->setChecked((mTimerMask & 0x1) != 0);
    ui.timer1Button->setChecked((mTimerMask & 0x2) != 0);
    ui.timer2Button->setChecked((mTimerMask & 0x4) != 0);
    ui.timer3Button->setChecked((mTimerMask & 0x8) != 0);
    ui.timer4Button->setChecked((mTimerMask & 0x10) != 0);

    switch(mAttackCompare)
    {
    case ECompareEqual: ui.attackStackedWidget->setCurrentIndex(1); break;
    case ECompareEqualOrLesser: ui.attackStackedWidget->setCurrentIndex(2); break;
    default: ui.attackStackedWidget->setCurrentIndex(0); break;
    }
    switch(mHpCompare)
    {
    case ECompareEqual: ui.hpStackedWidget->setCurrentIndex(1); break;
    case ECompareEqualOrLesser: ui.hpStackedWidget->setCurrentIndex(2); break;
    default: ui.hpStackedWidget->setCurrentIndex(0); break;
    }
    ui.attackSlider->setValue(mAttackValue);
    ui.hpSlider->setValue(mHpValue);

    if (mCheckUpgradeLevel)
    {
        switch(mUpgradeLevel)
        {
        case 0: ui.upgradeStackedWidget->setCurrentIndex(1); break;
        case 1: ui.upgradeStackedWidget->setCurrentIndex(2); break;
        case 2: ui.upgradeStackedWidget->setCurrentIndex(3); break;
        default: ui.upgradeStackedWidget->setCurrentIndex(0); break;
        }
    }
    else
    {
        ui.upgradeStackedWidget->setCurrentIndex(0);
    }
    ui.uniqueButton->setChecked(mIsUnique);
}

void CCardSearchParameters::fetchFromSettings(QSettings &settings)
{
    settings.beginGroup("searchOptions");
    mName = settings.value("name", mName).toString();
    mLwCsName = mName.toLower();
    mSkill = settings.value("skill", mSkill).toStringList();
    mLwCsSkill.clear();
    for (QStringList::const_iterator i = mSkill.begin(); i != mSkill.end(); ++i)
    {
        mLwCsSkill.push_back(i->toLower());
    }
    mRarityMask = settings.value("rarityMask", mRarityMask).toInt();
    mTypeMask = settings.value("typeMask", mTypeMask).toInt();
    mFactionMask = settings.value("factionMask", mFactionMask).toInt();
    mTimerMask = settings.value("timerMask", mTimerMask).toInt();
    mAttackCompare = static_cast<EComparisonMethod>(settings.value("attackCompare").toInt());
    mHpCompare = static_cast<EComparisonMethod>(settings.value("hpCompare").toInt());
    mAttackValue = settings.value("attackValue", mAttackValue).toInt();
    mHpValue = settings.value("hpValue", mHpValue).toInt();  
    mUpgradeLevel = settings.value("upgradeLevel", mUpgradeLevel).toInt();
    mCheckUpgradeLevel = settings.value("checkUpgradeLevel", mCheckUpgradeLevel).toBool();
    mIsUnique = settings.value("checkUnique", mIsUnique).toBool();
    settings.endGroup();
}

void CCardSearchParameters::updateSettings(QSettings &settings) const
{
    settings.beginGroup("searchOptions");
    settings.setValue("name", mName);
    settings.setValue("skill", mSkill);
    settings.setValue("rarityMask", mRarityMask);
    settings.setValue("typeMask", mTypeMask);
    settings.setValue("factionMask", mFactionMask);
    settings.setValue("timerMask", mTimerMask);
    settings.setValue("attackCompare", static_cast<int>(mAttackCompare));
    settings.setValue("hpCompare", static_cast<int>(mHpCompare));
    settings.setValue("attackValue", mAttackValue);
    settings.setValue("hpValue", mHpValue);
    settings.setValue("upgradeLevel", mUpgradeLevel);
    settings.setValue("checkUpgradeLevel", mCheckUpgradeLevel);
    settings.setValue("checkUnique", mIsUnique);
    settings.endGroup();
}

bool CCardSearchParameters::checkCard(const CCard &card, int &/*num*/) const
{
    bool pass(true);

    if (pass)
    {
        pass = card.getLwCsName().contains(mLwCsName);
    }

    if (pass)
    {
        switch(mAttackCompare)
        {
        case ECompareLarger: pass = qMax(0, card.getAttack()) > mAttackValue; break;
        case ECompareEqualOrLarger: pass = qMax(0, card.getAttack()) >= mAttackValue; break;
        case ECompareEqual: pass = qMax(0, card.getAttack()) == mAttackValue; break;
        case ECompareEqualOrLesser: pass = qMax(0, card.getAttack()) <= mAttackValue; break;
        case ECompareLesser: pass = qMax(0, card.getAttack()) < mAttackValue; break;
        default: pass = false; break;
        }
    }

    if (pass)
    {
        switch(mHpCompare)
        {
        case ECompareLarger: pass = qMax(0, card.getHealth()) > mHpValue; break;
        case ECompareEqualOrLarger: pass = qMax(0, card.getHealth()) >= mHpValue; break;
        case ECompareEqual: pass = qMax(0, card.getHealth()) == mHpValue; break;
        case ECompareEqualOrLesser: pass = qMax(0, card.getHealth()) <= mHpValue; break;
        case ECompareLesser: pass = qMax(0, card.getHealth()) < mHpValue; break;
        default: pass = false; break;
        }
    }


    if (pass && mRarityMask != 0)
    {
        switch (card.getRarity())
        {
        case ELegendaryRarity: pass = (mRarityMask & 0x1) != 0; break;
        case ERareRarity: pass = (mRarityMask & 0x2) != 0; break;
        case EUncommonRarity: pass = (mRarityMask & 0x4) != 0; break;
        case ECommonRarity: pass = (mRarityMask & 0x8) != 0; break;
        default: pass = false; break;
        }
    }

    if (pass && mTypeMask != 0)
    {
        switch (card.getType())
        {
        case EAssaultType: pass = (mTypeMask & 0x1) != 0; break;
        case EStructureType: pass = (mTypeMask & 0x2) != 0; break;
        case ECommanderType: pass = (mTypeMask & 0x4) != 0; break;
        case EActionType: pass = (mTypeMask & 0x8) != 0; break;
        default: pass = false; break;
        }
    }

    if (pass && mFactionMask != 0)
    {
        switch (card.getFaction())
        {
        case EBloodthirstyFaction: pass = (mFactionMask & 0x1) != 0; break;
        case EImperialFaction: pass = (mFactionMask & 0x2) != 0; break;
        case ERaiderFaction: pass = (mFactionMask & 0x4) != 0; break;
        case ERighteousFaction: pass = (mFactionMask & 0x8) != 0; break;
        case EXenoFaction: pass = (mFactionMask & 0x10) != 0; break;
        default: pass = false; break;
        }
    }

    if (pass && mTimerMask != 0)
    {
        switch (card.getDelay())
        {
        case 0: pass = (mTimerMask & 0x1) != 0; break;
        case 1: pass = (mTimerMask & 0x2) != 0; break;
        case 2: pass = (mTimerMask & 0x4) != 0; break;
        case 3: pass = (mTimerMask & 0x8) != 0; break;
        case 4: pass = (mTimerMask & 0x10) != 0; break;
        default: pass = false; break;
        }
    }

    if (pass && mCheckUpgradeLevel)
    {
        switch(mUpgradeLevel)
        {
        case 0: pass = card.getUpgradeLevel() == EUpgradeNotAvailable; break;
        case 1: pass = card.getUpgradeLevel() == EUpgradeLevel1; break;
        case 2: pass = card.getUpgradeLevel() == EUpgradeLevel2; break;
        default: pass = false; break;
        }
    }

    if (pass && mIsUnique)
    {
        pass = card.isUnique();
    }

    if (pass)
    {
        int skillPass(0);
        const TSkillList& skills = card.getSkills();
        for (TSkillList::const_iterator i = skills.begin(); skillPass < mSkill.size() && i != skills.end(); ++i)
        {
            const CSkill& curSkill = mCards.getSkillForId(i->getId());
            const QString &curSkillSig = curSkill.makeSignature(*i).toLower();
            for(QStringList::const_iterator j = mLwCsSkill.begin(); j != mLwCsSkill.end(); ++j)
            {
                if (curSkillSig.contains(*j))
                {
                    ++skillPass;
                    break;
                }
            }
        }
        pass = (skillPass == mSkill.size());
    }

    return pass;

}

