#include "CCardFilterParameters.h"
#include "CCardTable.h"
#include "ui_CardFilterWidget.h"


CCardFilterParameters::CCardFilterParameters()
: mCards(CCardTable::getCardTable())
, mIsAssaultAllowed()
, mIsStructureAllowed()
, mIsCommanderAllowed()
, mIsActionAllowed()
, mIsHealthAllowed()
, mComplement()
, mWhiteList()
, mBlackList()
, mIsFilterOptionEnabled(true)
, mIsWhiteListEnabled(true)
, mIsBlackListEnabled(true)
, mIsCompletionEnabled(true)
{
    for (int i = 0; i < NUM_RARITY; ++i)
    {
        mIsAssaultAllowed[i] = true;
        mIsStructureAllowed[i] = true;
        mIsCommanderAllowed[i] = true;
        mIsActionAllowed[i] = true;
        mIsHealthAllowed[i] = true;
    }
    for (int i = 0; i < NUM_SET; ++i)
    {
        mComplement[i] = 0;
    }
    mComplement[0] = 10;
    mComplement[1] = 5;
    mComplement[2] = 5;
    mComplement[3] = 3;
    mComplement[12] = 10;
    mComplement[13] = 3;
    mComplement[14] = 10;
}

void CCardFilterParameters::fetchFromUi(const Ui::CardFilterWidget &ui)
{
    mIsAssaultAllowed[0] = ui.assaultLegendaryBox->isChecked();
    mIsAssaultAllowed[1] = ui.assaultRareBox->isChecked();
    mIsAssaultAllowed[2] = ui.assaultUncommonBox->isChecked();
    mIsAssaultAllowed[3] = ui.assaultCommonBox->isChecked();

    mIsStructureAllowed[0] = ui.structureLegendaryBox->isChecked();
    mIsStructureAllowed[1] = ui.structureRareBox->isChecked();
    mIsStructureAllowed[2] = ui.structureUncommonBox->isChecked();
    mIsStructureAllowed[3] = ui.structureCommonBox->isChecked();

    mIsCommanderAllowed[0] = ui.commanderLegendaryBox->isChecked();
    mIsCommanderAllowed[1] = ui.commanderRareBox->isChecked();
    mIsCommanderAllowed[2] = ui.commanderUncommonBox->isChecked();
    mIsCommanderAllowed[3] = ui.commanderCommonBox->isChecked();

    mIsActionAllowed[0] = ui.actionLegendaryBox->isChecked();
    mIsActionAllowed[1] = ui.actionRareBox->isChecked();
    mIsActionAllowed[2] = ui.actionUncommonBox->isChecked();
    mIsActionAllowed[3] = ui.actionCommonBox->isChecked();

    mIsHealthAllowed[0] = ui.health4Box->isChecked();
    mIsHealthAllowed[1] = ui.health3Box->isChecked();
    mIsHealthAllowed[2] = ui.health2Box->isChecked();
    mIsHealthAllowed[3] = ui.health1Box->isChecked();

    mComplement[0] = ui.standardSpinBox->value();
    mComplement[1] = ui.enclaveSpinBox->value();
    mComplement[2] = ui.nexusSpinBox->value();
    mComplement[3] = ui.blightSpinBox->value();
    mComplement[4] = ui.puritySpinBox->value();
    mComplement[5] = ui.homeWorldSpinBox->value();
    mComplement[6] = ui.phobosSpinBox->value();
    mComplement[7] = ui.phobosAfterMathSpinBox->value();
    mComplement[8] = ui.awakeningSpinBox->value();
    mComplement[9] = ui.terminusSpinBox->value();
    mComplement[10] = ui.occupationSpinBox->value();
    mComplement[11] = ui.worldShipSpinBox->value();
    mComplement[12] = ui.rewardSpinBox->value();
    mComplement[13] = ui.promotionalSpinBox->value();
    mComplement[14] = ui.upgradedSpinBox->value();

    mWhiteList.clear();
    QStringList whiteList = ui.whiteListEdit->toPlainText().split("\n", QString::SkipEmptyParts);
    for(QStringList::iterator i = whiteList.begin(); i != whiteList.end(); ++i)
    {
        QStringList curCardTokens = (*i).split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
        if (curCardTokens.size() > 0)
        {
            int num(-1);
            if (curCardTokens.size() > 1)
            {
                bool ok(true);
                num = curCardTokens.at(1).toInt(&ok);
            }
            const CCard &card = mCards.getCardForName(curCardTokens.at(0).trimmed());
            if (card.isValid())
            {
                mWhiteList.insert(card.getName(), num);
            }
        }
    }

    mBlackList.clear();
    QStringList blackList = ui.blackListEdit->toPlainText().split("\n", QString::SkipEmptyParts);
    for(QStringList::iterator i = blackList.begin(); i != blackList.end(); ++i)
    {
        QStringList curCardTokens = (*i).split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
        if (curCardTokens.size() > 0)
        {
            int num(-1);
            if (curCardTokens.size() > 1)
            {
                bool ok(true);
                num = curCardTokens.at(1).toInt(&ok);
            }
            const CCard &card = mCards.getCardForName(curCardTokens.at(0).trimmed());
            if (card.isValid())
            {
                mBlackList.insert(card.getName(), num);
            }
        }
    }

    mIsFilterOptionEnabled = ui.filterOptionsBox->isChecked();
    mIsWhiteListEnabled = ui.whiteListBox->isChecked();
    mIsBlackListEnabled = ui.blackListBox->isChecked();
    mIsCompletionEnabled = ui.complementsBox->isChecked();
}

void CCardFilterParameters::updateUi(Ui::CardFilterWidget &ui) const
{
    ui.assaultLegendaryBox->setChecked(mIsAssaultAllowed[0]);
    ui.assaultRareBox->setChecked(mIsAssaultAllowed[1]);
    ui.assaultUncommonBox->setChecked(mIsAssaultAllowed[2]);
    ui.assaultCommonBox->setChecked(mIsAssaultAllowed[3]);

    ui.structureLegendaryBox->setChecked(mIsStructureAllowed[0]);
    ui.structureRareBox->setChecked(mIsStructureAllowed[1]);
    ui.structureUncommonBox->setChecked(mIsStructureAllowed[2]);
    ui.structureCommonBox->setChecked(mIsStructureAllowed[3]);

    ui.commanderLegendaryBox->setChecked(mIsCommanderAllowed[0]);
    ui.commanderRareBox->setChecked(mIsCommanderAllowed[1]);
    ui.commanderUncommonBox->setChecked(mIsCommanderAllowed[2]);
    ui.commanderCommonBox->setChecked(mIsCommanderAllowed[3]);

    ui.actionLegendaryBox->setChecked(mIsActionAllowed[0]);
    ui.actionRareBox->setChecked(mIsActionAllowed[1]);
    ui.actionUncommonBox->setChecked(mIsActionAllowed[2]);
    ui.actionCommonBox->setChecked(mIsActionAllowed[3]);

    ui.health4Box->setChecked(mIsHealthAllowed[0]);
    ui.health3Box->setChecked(mIsHealthAllowed[1]);
    ui.health2Box->setChecked(mIsHealthAllowed[2]);
    ui.health1Box->setChecked(mIsHealthAllowed[3]);

    QStringList whiteList;
    for(QHash<QString, int>::const_iterator i = mWhiteList.begin(); i != mWhiteList.end(); ++i)
    {
        whiteList.push_back(i.key());
    }
    whiteList.sort();
    ui.whiteListEdit->setPlainText(whiteList.join("\n"));

    QStringList blackList;
    for(QHash<QString, int>::const_iterator i = mBlackList.begin(); i != mBlackList.end(); ++i)
    {
        blackList.push_back(i.key());
    }
    blackList.sort();
    ui.blackListEdit->setPlainText(blackList.join("\n"));

    ui.standardSpinBox->setValue(mComplement[0]);
    ui.enclaveSpinBox->setValue(mComplement[1]);
    ui.nexusSpinBox->setValue(mComplement[2]);
    ui.blightSpinBox->setValue(mComplement[3]);
    ui.puritySpinBox->setValue(mComplement[4]);
    ui.homeWorldSpinBox->setValue(mComplement[5]);
    ui.phobosSpinBox->setValue(mComplement[6]);
    ui.phobosAfterMathSpinBox->setValue(mComplement[7]);
    ui.awakeningSpinBox->setValue(mComplement[8]);
    ui.terminusSpinBox->setValue(mComplement[9]);
    ui.occupationSpinBox->setValue(mComplement[10]);
    ui.worldShipSpinBox->setValue(mComplement[11]);
    ui.rewardSpinBox->setValue(mComplement[12]);
    ui.promotionalSpinBox->setValue(mComplement[13]);
    ui.upgradedSpinBox->setValue(mComplement[14]);

    ui.filterOptionsBox->setChecked(mIsFilterOptionEnabled);
    ui.whiteListBox->setChecked(mIsWhiteListEnabled);
    ui.blackListBox->setChecked(mIsBlackListEnabled);
    ui.complementsBox->setChecked(mIsCompletionEnabled);
}

void CCardFilterParameters::fetchFromSettings(QSettings &settings)
{
    settings.beginGroup("filterOptions");
    mIsAssaultAllowed[0] = settings.value("allowAssaultsLegendary", mIsAssaultAllowed[0]).toBool();
    mIsAssaultAllowed[1] = settings.value("allowAssaultsRare", mIsAssaultAllowed[1]).toBool();
    mIsAssaultAllowed[2] = settings.value("allowAssaultsUncommon", mIsAssaultAllowed[2]).toBool();
    mIsAssaultAllowed[3] = settings.value("allowAssaultsCommon", mIsAssaultAllowed[3]).toBool();

    mIsStructureAllowed[0] = settings.value("allowStructuresLegendary", mIsStructureAllowed[0]).toBool();
    mIsStructureAllowed[1] = settings.value("allowStructuresRare", mIsStructureAllowed[1]).toBool();
    mIsStructureAllowed[2] = settings.value("allowStructuresUncommon", mIsStructureAllowed[2]).toBool();
    mIsStructureAllowed[3] = settings.value("allowStructuresCommon", mIsStructureAllowed[3]).toBool();

    mIsCommanderAllowed[0] = settings.value("allowCommandersLegendary", mIsCommanderAllowed[0]).toBool();
    mIsCommanderAllowed[1] = settings.value("allowCommandersRare", mIsCommanderAllowed[1]).toBool();
    mIsCommanderAllowed[2] = settings.value("allowCommandersUncommon", mIsCommanderAllowed[2]).toBool();
    mIsCommanderAllowed[3] = settings.value("allowCommandersCommon", mIsCommanderAllowed[3]).toBool();

    mIsActionAllowed[0] = settings.value("allowActionsLegendary", mIsActionAllowed[0]).toBool();
    mIsActionAllowed[1] = settings.value("allowActionsRare", mIsActionAllowed[1]).toBool();
    mIsActionAllowed[2] = settings.value("allowActionsUncommon", mIsActionAllowed[2]).toBool();
    mIsActionAllowed[3] = settings.value("allowActionsCommon", mIsActionAllowed[3]).toBool();

    mIsHealthAllowed[0] = settings.value("allowHealth4Plus", mIsHealthAllowed[0]).toBool();
    mIsHealthAllowed[1] = settings.value("allowHealth3", mIsHealthAllowed[1]).toBool();
    mIsHealthAllowed[2] = settings.value("allowHealth2", mIsHealthAllowed[2]).toBool();
    mIsHealthAllowed[3] = settings.value("allowHealth1", mIsHealthAllowed[3]).toBool();

    mWhiteList.clear();
    QStringList defaultList;
    defaultList
            << "Acid Athenor" << "Asylum" << "Atelier" << "Blood Wall" << "Boot Camp"
            << "Daizon" << "Egg Infector" << "Enclave Warp Gate" << "Fortified Cannons"
            << "Gene Reader" << "Gruesome Crawler" << "Heli-Duster" << "Hunter" << "Hydraulis"
            << "Irradiated Infantry" << "Meteor" << "Mirror Wall" << "Mizar VIII"
            << "Nephilim Knight" << "Neverender" << "Offshore Platform" << "Outfitted Scow"
            << "Pathrazer" << "Patriarch" << "Patrol Cruiser" << "Phantasm" << "Phaseid"
            << "Predator" << "Prism" << "Reclamax" << "Ryoko" << "Shaded Hollow"
            << "Shrapnel Engine" << "Shrine of Hope" << "Speculus" << "Stealthy Niaq"
            << "Subterfuge" << "Sundering Ogre" << "Tiamat" << "Toxic Cannon" << "Withersnap"
            << "Valentina";
    QStringList whiteList = settings.value("whiteList", defaultList).toStringList();
    for(QStringList::iterator i = whiteList.begin(); i != whiteList.end(); ++i)
    {
        QStringList curCardTokens = (*i).split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
        if (curCardTokens.size() > 0)
        {
            const CCard &card = mCards.getCardForName(curCardTokens.at(0).trimmed());
            if (card.isValid())
            {
                mWhiteList.insert(card.getName(), 0);
            }
        }
    }

    mBlackList.clear();
    QStringList blackList = settings.value("blackList").toStringList();
    for(QStringList::iterator i = blackList.begin(); i != blackList.end(); ++i)
    {
        QStringList curCardTokens = (*i).split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
        if (curCardTokens.size() > 0)
        {
            const CCard &card = mCards.getCardForName(curCardTokens.at(0).trimmed());
            if (card.isValid())
            {
                mBlackList.insert(card.getName(), 0);
            }
        }
    }

    mComplement[0] = settings.value("standardSetComplement", mComplement[0]).toInt();
    mComplement[1] = settings.value("enclaveSetComplement", mComplement[1]).toInt();
    mComplement[2] = settings.value("nexusSetComplement", mComplement[2]).toInt();
    mComplement[3] = settings.value("blightSetComplement", mComplement[3]).toInt();
    mComplement[4] = settings.value("puritySetComplement", mComplement[4]).toInt();
    mComplement[5] = settings.value("homeworldSetComplement", mComplement[5]).toInt();
    mComplement[6] = settings.value("phobosSetComplement", mComplement[6]).toInt();
    mComplement[7] = settings.value("phobosAfermathSetComplement", mComplement[7]).toInt();
    mComplement[8] = settings.value("awakeningSetComplement", mComplement[8]).toInt();
    mComplement[9] = settings.value("terminusSetComplement", mComplement[9]).toInt();
    mComplement[10] = settings.value("occupationSetComplement", mComplement[10]).toInt();
    mComplement[11] = settings.value("worldshipSetComplement", mComplement[11]).toInt();
    mComplement[12] = settings.value("rewardSetComplement", mComplement[12]).toInt();
    mComplement[13] = settings.value("promotionalSetComplement", mComplement[13]).toInt();
    mComplement[14] = settings.value("upgradedSetComplement", mComplement[14]).toInt();


    mIsFilterOptionEnabled = settings.value("isFilterOptionEnabled", mIsFilterOptionEnabled).toBool();
    mIsWhiteListEnabled = settings.value("isWhiteListEnabled", mIsWhiteListEnabled).toBool();
    mIsBlackListEnabled = settings.value("isBlackListEnabled", mIsBlackListEnabled).toBool();
    mIsCompletionEnabled = settings.value("isCompletionEnabled", mIsCompletionEnabled).toBool();

    settings.endGroup();
}

void CCardFilterParameters::updateSettings(QSettings &settings) const
{
    settings.beginGroup("filterOptions");

    settings.setValue("allowAssaultsLegendary", mIsAssaultAllowed[0]);
    settings.setValue("allowAssaultsRare", mIsAssaultAllowed[1]);
    settings.setValue("allowAssaultsUncommon", mIsAssaultAllowed[2]);
    settings.setValue("allowAssaultsCommon", mIsAssaultAllowed[3]);

    settings.setValue("allowStructuresLegendary", mIsStructureAllowed[0]);
    settings.setValue("allowStructuresRare", mIsStructureAllowed[1]);
    settings.setValue("allowStructuresUncommon", mIsStructureAllowed[2]);
    settings.setValue("allowStructuresCommon", mIsStructureAllowed[3]);

    settings.setValue("allowCommandersLegendary", mIsCommanderAllowed[0]);
    settings.setValue("allowCommandersRare", mIsCommanderAllowed[1]);
    settings.setValue("allowCommandersUncommon", mIsCommanderAllowed[2]);
    settings.setValue("allowCommandersCommon", mIsCommanderAllowed[3]);

    settings.setValue("allowActionsLegendary", mIsActionAllowed[0]);
    settings.setValue("allowActionsRare", mIsActionAllowed[1]);
    settings.setValue("allowActionsUncommon", mIsActionAllowed[2]);
    settings.setValue("allowActionsCommon", mIsActionAllowed[3]);

    settings.setValue("allowHealth4Plus", mIsHealthAllowed[0]);
    settings.setValue("allowHealth3", mIsHealthAllowed[1]);
    settings.setValue("allowHealth2", mIsHealthAllowed[2]);
    settings.setValue("allowHealth1", mIsHealthAllowed[3]);

    QStringList whiteList;
    for(QHash<QString, int>::const_iterator i = mWhiteList.begin(); i != mWhiteList.end(); ++i)
    {
        whiteList.push_back(i.key());
    }
    settings.setValue("whiteList", whiteList);

    QStringList blackList;
    for(QHash<QString, int>::const_iterator i = mBlackList.begin(); i != mBlackList.end(); ++i)
    {
        blackList.push_back(i.key());
    }
    settings.setValue("blackList", blackList);

    settings.setValue("standardSetComplement", mComplement[0]);
    settings.setValue("enclaveSetComplement", mComplement[1]);
    settings.setValue("nexusSetComplement", mComplement[2]);
    settings.setValue("blightSetComplement", mComplement[3]);
    settings.setValue("puritySetComplement", mComplement[4]);
    settings.setValue("homeworldSetComplement", mComplement[5]);
    settings.setValue("phobosSetComplement", mComplement[6]);
    settings.setValue("phobosAfermathSetComplement", mComplement[7]);
    settings.setValue("awakeningSetComplement", mComplement[8]);
    settings.setValue("terminusSetComplement", mComplement[9]);
    settings.setValue("occupationSetComplement", mComplement[10]);
    settings.setValue("worldshipSetComplement", mComplement[11]);
    settings.setValue("rewardSetComplement", mComplement[12]);
    settings.setValue("promotionalSetComplement", mComplement[13]);
    settings.setValue("upgradedSetComplement", mComplement[14]);

    settings.setValue("isFilterOptionEnabled", mIsFilterOptionEnabled);
    settings.setValue("isWhiteListEnabled", mIsWhiteListEnabled);
    settings.setValue("isBlackListEnabled", mIsBlackListEnabled);
    settings.setValue("isCompletionEnabled", mIsCompletionEnabled);

    settings.endGroup();
}

bool CCardFilterParameters::checkCard(const CCard &card, int &num) const
{
    if (mIsCompletionEnabled)
    {
        switch (card.getSet())
        {
        case EEnclaveSet: num = qMax(num, mComplement[1]); break;
        case ENexusSet: num = qMax(num, mComplement[2]); break;
        case EBlightSet: num = qMax(num, mComplement[3]); break;
        case EPuritySet: num = qMax(num, mComplement[4]); break;
        case EHomeWorldSet: num = qMax(num, mComplement[5]); break;
        case EPhobosSet: num = qMax(num, mComplement[6]); break;
        case EPhobosAftermathSet: num = qMax(num, mComplement[7]); break;
        case EAwakeningSet: num = qMax(num, mComplement[8]); break;
        case ETerminusSet: num = qMax(num, mComplement[9]); break;
        case EOccupationSet: num = qMax(num, mComplement[10]); break;
        case EWorldshipSet: num = qMax(num, mComplement[11]); break;
        case EStandardSet: num = qMax(num, mComplement[0]); break;
        case ERewardSet: num = qMax(num, mComplement[12]); break;
        case EPromotionalSet: num = qMax(num, mComplement[13]); break;
        case EUpgradedSet: num = qMax(num, mComplement[14]); break;
        default: break;
        }
    }

    if (mIsBlackListEnabled)
    {
        QHash<QString, int>::const_iterator iBlack = mBlackList.find(card.getName());
        if (iBlack != mBlackList.end() && iBlack.key() == card.getName())
        {
            if (iBlack.value() > 0)
            {
                num -= iBlack.value();
            }
            else
            {
                num = 0;
            }
            if (num < 1)
            {
                return false;
            }
        }
    }

    if (mIsWhiteListEnabled)
    {
        QHash<QString, int>::const_iterator iWhite = mWhiteList.find(card.getName());
        if (iWhite != mWhiteList.end() && iWhite.key() == card.getName())
        {
            if (iWhite.value() > 0)
            {
                num = qMin(num, iWhite.value());
            }
            return true;
        }
    }

    if (mIsFilterOptionEnabled)
    {
        bool passRarity(true);
        switch (card.getType())
        {
        case EAssaultType:
            switch (card.getRarity())
            {
            case ECommonRarity: passRarity = mIsAssaultAllowed[3]; break;
            case EUncommonRarity: passRarity = mIsAssaultAllowed[2]; break;
            case ERareRarity: passRarity = mIsAssaultAllowed[1]; break;
            case ELegendaryRarity: passRarity = mIsAssaultAllowed[0]; break;
            default: break;
            }
            break;
        case ECommanderType:
            switch (card.getRarity())
            {
            case ECommonRarity: passRarity = mIsCommanderAllowed[3]; break;
            case EUncommonRarity: passRarity = mIsCommanderAllowed[2]; break;
            case ERareRarity: passRarity = mIsCommanderAllowed[1]; break;
            case ELegendaryRarity: passRarity = mIsCommanderAllowed[0]; break;
            default: break;
            }
            break;
        case EStructureType:
            switch (card.getRarity())
            {
            case ECommonRarity: passRarity = mIsStructureAllowed[3]; break;
            case EUncommonRarity: passRarity = mIsStructureAllowed[2]; break;
            case ERareRarity: passRarity = mIsStructureAllowed[1]; break;
            case ELegendaryRarity: passRarity = mIsStructureAllowed[0]; break;
            default: break;
            }
            break;
        case EActionType:
            switch (card.getRarity())
            {
            case ECommonRarity: passRarity = mIsActionAllowed[3]; break;
            case EUncommonRarity: passRarity = mIsActionAllowed[2]; break;
            case ERareRarity: passRarity = mIsActionAllowed[1]; break;
            case ELegendaryRarity: passRarity = mIsActionAllowed[0]; break;
            default: break;
            }
            break;
        default:
            break;
        }

        bool passHealth(true);
        if (card.getType() == EAssaultType)
        {
            switch (card.getHealth())
            {
            case 1: passHealth = mIsHealthAllowed[3]; break;
            case 2: passHealth = mIsHealthAllowed[2]; break;
            case 3: passHealth = mIsHealthAllowed[1]; break;
            default: passHealth = mIsHealthAllowed[0]; break;
            }
        }
        return passRarity && passHealth;
    }
    return true;
}
