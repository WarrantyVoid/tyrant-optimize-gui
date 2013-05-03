#include "CCardSearchParameters.h"
#include "ui_CardSearchWidget.h"
#include "model/CCard.h"
#include "model/CSkill.h"
#include "CCardTable.h"
#include <QLineEdit>

CCardSearchParameters::CCardSearchParameters()
: mCards(CCardTable::getCardTable())
, mName()
, mLwCsName()
, mSkill()
, mLwCsSkill()
, mRarityMask(0)
, mTypeMask(0)
, mMinAttack(0)
, mMinHp(0)
{
}

void CCardSearchParameters::fetchFromUi(const Ui::CardSearchWidget &ui)
{
    mName = ui.nameBox->currentText();
    mLwCsName = mName.toLower();
    mSkill = ui.skillBox->currentText().split("+", QString::SkipEmptyParts);
    mLwCsSkill.clear();
    for (QStringList::const_iterator i = mSkill.begin(); i != mSkill.end(); ++i)
    {
        mLwCsSkill.push_back(i->toLower());
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

    mMinAttack = ui.attackSlider->value();
    mMinHp = ui.hpSlider->value();
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

    ui.attackSlider->setValue(mMinAttack);
    ui.hpSlider->setValue(mMinHp);
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
    mMinAttack = settings.value("minAttack", mMinAttack).toInt();
    mMinHp = settings.value("minHp", mMinHp).toInt();
    settings.endGroup();
}

void CCardSearchParameters::updateSettings(QSettings &settings) const
{
    settings.beginGroup("searchOptions");
    settings.setValue("name", mName);
    settings.setValue("skill", mSkill);
    settings.setValue("rarityMask", mRarityMask);
    settings.setValue("typeMask", mTypeMask);
    settings.setValue("minAttack", mMinAttack);
    settings.setValue("minHp", mMinHp);
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
        pass = qMax(0, card.getAttack()) >= mMinAttack;
    }

    if (pass)
    {
        pass = qMax(0,card.getHealth()) >= mMinHp;
    }


    if (pass)
    {
        TSearchMask rarityMask = (mRarityMask == 0) ? 0xf : mRarityMask;
        switch (card.getRarity())
        {
        case ECommonRarity: pass = (rarityMask & 0x8) != 0; break;
        case EUncommonRarity: pass = (rarityMask & 0x4) != 0; break;
        case ERareRarity : pass = (rarityMask & 0x2) != 0; break;
        case ELegendaryRarity: pass = (rarityMask & 0x1) != 0; break;
        default: break;
        }
    }

    if (pass)
    {
        TSearchMask typeMask = (mTypeMask == 0) ? 0xf : mTypeMask;
        switch (card.getType())
        {
        case EAssaultType: pass = (typeMask & 0x1) != 0; break;
        case ECommanderType: pass = (typeMask & 0x4) != 0; break;
        case EStructureType : pass = (typeMask & 0x2) != 0; break;
        case EActionType: pass = (typeMask & 0x8) != 0; break;
        default: break;
        }
    }

    if (pass)
    {
        int skillPass(0);
        const TCardSkills& skills = card.getSkills();
        for (TCardSkills::const_iterator i = skills.begin(); skillPass < mSkill.size() && i != skills.end(); ++i)
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

