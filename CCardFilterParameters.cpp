#include "CCardFilterParameters.h"
#include "model/CCardTable.h"
#include "model/CDeck.h"
#include "ui_CardFilterWidget.h"

QStringList CCardFilterParameters::sDefaultList;

CCardFilterParameters::CCardFilterParameters()
: mIsAssaultAllowed()
, mIsStructureAllowed()
, mIsCommanderAllowed()
, mIsActionAllowed()
, mIsHealthAllowed()
, mIsAttackAllowed()
, mIsDelayAllowed()
, mIsFactionAllowed()
, mComplement()
, mWhiteList()
, mBlackList()
, mBlockList()
, mIsFilterOptionEnabled(true)
, mIsAssaultOptionEnabled(true)
, mIsWhiteListEnabled(true)
, mIsBlackListEnabled(true)
, mIsBlockListEnabled(true)
, mIsCompletionEnabled(false)
{
    if (sDefaultList.isEmpty())
    {
        sDefaultList
        << "Acid Athenor" << "Asylum" << "Atelier" << "Battle Mount" << "Blitz Precursor"
        << "Blood Rider" << "Blood Wall" << "Boot Camp" << "Clipper" << "Daizon"
        << "Egg Infector" << "Enclave Warp Gate" << "Exoskeleton" << "Fortified Cannons"
        << "Fury Walker" << "Gene Reader" << "Groteske" << "Gruesome Crawler"
        << "Heli-Duster" << "Holy Beacon <<" "Hunter" << "Hydraulis" << "Impulse Walker"
        << "Irradiated Infantry" << "Meteor" << "Mirror Wall" << "Mizar VIII"
        << "Nephilim Knight" << "Neverender" << "Offshore Platform" << "Outfitted Scow"
        << "Partisan" << "Pathrazer" << "Patriarch" << "Patrol Cruiser" << "Phantasm"
        << "Phaseid" << "Photon Dish" << "Predator" << "Prism" << "Quadrishot"
        << "Reclamax" << "Ryoko" << "Quilled Blaster" << "Serak"  << "Shaded Hollow"
        << "Shrapnel Engine" << "Shrine of Hope" << "Speculus" << "Stealthy Niaq"
        << "Subterfuge" << "Sundering Ogre" << "Tiamat" << "Toxic Cannon"
        << "Withersnap" << "Valentina";
    }
    reset();
}

void CCardFilterParameters::reset()
{
    mIsFilterOptionEnabled = true;
    mIsAssaultOptionEnabled = true;
    mIsWhiteListEnabled = true;
    mIsBlackListEnabled = true;
    mIsBlockListEnabled = true;
    mIsCompletionEnabled = false;

    for (int i = 0; i < NUM_RARITY; ++i)
    {
        mIsAssaultAllowed[i] = (i < 2);
        mIsStructureAllowed[i] = (i < 2);
        mIsCommanderAllowed[i] = (i < 3);
        mIsActionAllowed[i] = (i < 2);
    }
    for (int i = 0; i < NUM_HEALTH; ++i)
    {
        mIsHealthAllowed[i] = (i > 2);
    }
    for (int i = 0; i < NUM_ATTACK; ++i)
    {
        mIsAttackAllowed[i] = true;
    }
    for (int i = 0; i < NUM_DELAY; ++i)
    {
        mIsDelayAllowed[i] = true;
    }
    for (int i = 0; i < NUM_FACTION; ++i)
    {
        mIsFactionAllowed[i] = true;
    }
    for (int i = 0; i < NUM_SET; ++i)
    {
        mComplement[i] = 0;
    }

    mComplement[0] = 5;
    mComplement[1] = 3;
    mComplement[2] = 3;
    mComplement[3] = 3;
    mComplement[13] = 3;
    mComplement[14] = 3;
    mComplement[15] = 3;

    mWhiteList.clear();
    CCardTable &cardTable = CCardTable::getCardTable();
    for(QStringList::iterator i = sDefaultList.begin(); i != sDefaultList.end(); ++i)
    { 
        const CCard &card = cardTable.getCardForName(*i);
        if (card.isValid())
        {
            mWhiteList.insert(card.getName());
        }
    }

    mBlackList.clear();
}

bool CCardFilterParameters::fetchFromUi(const Ui::CardFilterWidget &ui)
{
    bool result(true);

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

    mIsHealthAllowed[0] = ui.health1Box->isChecked();
    mIsHealthAllowed[1] = ui.health2Box->isChecked();
    mIsHealthAllowed[2] = ui.health3Box->isChecked();
    mIsHealthAllowed[3] = ui.health4Box->isChecked();
    mIsHealthAllowed[4] = ui.health5Box->isChecked();

    mIsAttackAllowed[0] = ui.attack0Box->isChecked();
    mIsAttackAllowed[1] = ui.attack1Box->isChecked();
    mIsAttackAllowed[2] = ui.attack2Box->isChecked();
    mIsAttackAllowed[3] = ui.attack3Box->isChecked();
    mIsAttackAllowed[4] = ui.attack4Box->isChecked();

    mIsDelayAllowed[0] = ui.delay0Box->isChecked();
    mIsDelayAllowed[1] = ui.delay1Box->isChecked();
    mIsDelayAllowed[2] = ui.delay2Box->isChecked();
    mIsDelayAllowed[3] = ui.delay3Box->isChecked();
    mIsDelayAllowed[4] = ui.delay4Box->isChecked();

    mIsFactionAllowed[0] = ui.factionBloodthirstyBox->isChecked();
    mIsFactionAllowed[1] = ui.factionImperialBox->isChecked();
    mIsFactionAllowed[2] = ui.factionRaiderBox->isChecked();
    mIsFactionAllowed[3] = ui.factionRighteousBox->isChecked();
    mIsFactionAllowed[4] = ui.factionXenoBox->isChecked();

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
    mComplement[12] = ui.flashPointSpinBox->value();
    mComplement[13] = ui.rewardSpinBox->value();
    mComplement[14] = ui.promotionalSpinBox->value();
    mComplement[15] = ui.upgradedSpinBox->value();

    CCardTable &cardTable = CCardTable::getCardTable();
    mWhiteList.clear();
    QStringList whiteList = ui.whiteListEdit->toPlainText().split("\n", QString::SkipEmptyParts);
    for(QStringList::iterator i = whiteList.begin(); i != whiteList.end(); ++i)
    {
        const CCard &card = cardTable.getCardForName(*i);
        if (card.isValid())
        {
            mWhiteList.insert(card.getName());
        }
        else
        {
            result = false;
        }
    }

    mBlackList.clear();
    QStringList blackList = ui.blackListEdit->toPlainText().split("\n", QString::SkipEmptyParts);
    for(QStringList::iterator i = blackList.begin(); i != blackList.end(); ++i)
    {
        const CCard &card = cardTable.getCardForName(*i);
        if (card.isValid())
        {
            mBlackList.insert(card.getName());
        }
        else
        {
            result = false;
        }
    }

    mIsFilterOptionEnabled = ui.filterOptionsBox->isChecked();
    mIsAssaultOptionEnabled = ui.assaultOptionsBox->isChecked();
    mIsWhiteListEnabled = ui.whiteListBox->isChecked();
    mIsBlackListEnabled = ui.blackListBox->isChecked();
    mIsBlockListEnabled = ui.blockListBox->isChecked();
    mIsCompletionEnabled = ui.complementsBox->isChecked();

    return result;
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

    ui.health1Box->setChecked(mIsHealthAllowed[0]);
    ui.health2Box->setChecked(mIsHealthAllowed[1]);
    ui.health3Box->setChecked(mIsHealthAllowed[2]);
    ui.health4Box->setChecked(mIsHealthAllowed[3]);
    ui.health5Box->setChecked(mIsHealthAllowed[4]);

    ui.attack0Box->setChecked(mIsAttackAllowed[0]);
    ui.attack1Box->setChecked(mIsAttackAllowed[1]);
    ui.attack2Box->setChecked(mIsAttackAllowed[2]);
    ui.attack3Box->setChecked(mIsAttackAllowed[3]);
    ui.attack4Box->setChecked(mIsAttackAllowed[4]);

    ui.delay0Box->setChecked(mIsDelayAllowed[0]);
    ui.delay1Box->setChecked(mIsDelayAllowed[1]);
    ui.delay2Box->setChecked(mIsDelayAllowed[2]);
    ui.delay3Box->setChecked(mIsDelayAllowed[3]);
    ui.delay4Box->setChecked(mIsDelayAllowed[4]);

    ui.factionBloodthirstyBox->setChecked(mIsFactionAllowed[0]);
    ui.factionImperialBox->setChecked(mIsFactionAllowed[1]);
    ui.factionRaiderBox->setChecked(mIsFactionAllowed[2]);
    ui.factionRighteousBox->setChecked(mIsFactionAllowed[3]);
    ui.factionXenoBox->setChecked(mIsFactionAllowed[4]);

    QStringList whiteList;
    getWhiteList(whiteList);
    whiteList.sort();
    ui.whiteListEdit->setPlainText(whiteList.join("\n"));

    QStringList blackList;
    getBlackList(blackList);
    blackList.sort();
    ui.blackListEdit->setPlainText(blackList.join("\n"));

    QStringList blockList;
    for(QHash<QString, int>::const_iterator i = mBlockList.begin(); i != mBlockList.end(); ++i)
    {
        QString curCard = listedCardToStr(TListedCard(i.key(), i.value()));
        if (!curCard.isEmpty())
        {
            blockList.push_back(curCard);
        }
    }
    blockList.sort();
    ui.blockListEdit->setPlainText(blockList.join("\n"));

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
    ui.flashPointSpinBox->setValue(mComplement[12]);
    ui.rewardSpinBox->setValue(mComplement[13]);
    ui.promotionalSpinBox->setValue(mComplement[14]);
    ui.upgradedSpinBox->setValue(mComplement[15]);

    ui.filterOptionsBox->setChecked(mIsFilterOptionEnabled);
    ui.assaultOptionsBox->setChecked(mIsAssaultOptionEnabled);
    ui.whiteListBox->setChecked(mIsWhiteListEnabled);
    ui.blackListBox->setChecked(mIsBlackListEnabled);
    ui.blockListBox->setChecked(mIsBlockListEnabled);
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

    mIsHealthAllowed[0] = settings.value("allowHealth1", mIsHealthAllowed[0]).toBool();
    mIsHealthAllowed[1] = settings.value("allowHealth2", mIsHealthAllowed[1]).toBool();
    mIsHealthAllowed[2] = settings.value("allowHealth3", mIsHealthAllowed[2]).toBool();
    mIsHealthAllowed[3] = settings.value("allowHealth4", mIsHealthAllowed[3]).toBool();
    mIsHealthAllowed[4] = settings.value("allowHealth5", mIsHealthAllowed[4]).toBool();

    mIsAttackAllowed[0] = settings.value("allowAttack0", mIsAttackAllowed[0]).toBool();
    mIsAttackAllowed[1] = settings.value("allowAttack1", mIsAttackAllowed[1]).toBool();
    mIsAttackAllowed[2] = settings.value("allowAttack2", mIsAttackAllowed[2]).toBool();
    mIsAttackAllowed[3] = settings.value("allowAttack3", mIsAttackAllowed[3]).toBool();
    mIsAttackAllowed[4] = settings.value("allowAttack4", mIsAttackAllowed[4]).toBool();

    mIsDelayAllowed[0] = settings.value("allowDelay0", mIsDelayAllowed[0]).toBool();
    mIsDelayAllowed[1] = settings.value("allowDelay1", mIsDelayAllowed[1]).toBool();
    mIsDelayAllowed[2] = settings.value("allowDelay2", mIsDelayAllowed[2]).toBool();
    mIsDelayAllowed[3] = settings.value("allowDelay3", mIsDelayAllowed[3]).toBool();
    mIsDelayAllowed[4] = settings.value("allowDelay4", mIsDelayAllowed[4]).toBool();

    mIsFactionAllowed[0] = settings.value("allowFactionBloodthirsty", mIsFactionAllowed[0]).toBool();
    mIsFactionAllowed[1] = settings.value("allowFactionImperial", mIsFactionAllowed[1]).toBool();
    mIsFactionAllowed[2] = settings.value("allowFactionRaider", mIsFactionAllowed[2]).toBool();
    mIsFactionAllowed[3] = settings.value("allowFactionRighteous", mIsFactionAllowed[3]).toBool();
    mIsFactionAllowed[4] = settings.value("allowFactionXeno", mIsFactionAllowed[4]).toBool();

    mWhiteList.clear();
    CCardTable &cardTable = CCardTable::getCardTable();
    QStringList whiteList = settings.value("whiteList", sDefaultList).toStringList();
    for(QStringList::iterator i = whiteList.begin(); i != whiteList.end(); ++i)
    {
        const CCard &card = cardTable.getCardForName(*i);
        if (card.isValid())
        {
            mWhiteList.insert(card.getName());
        }
    }

    mBlackList.clear();
    QStringList blackList = settings.value("blackList").toStringList();
    for(QStringList::iterator i = blackList.begin(); i != blackList.end(); ++i)
    {
        const CCard &card = cardTable.getCardForName(*i);
        if (card.isValid())
        {
            mBlackList.insert(card.getName());
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
    mComplement[12] = settings.value("flashpointSetComplement", mComplement[12]).toInt();
    mComplement[13] = settings.value("rewardSetComplement", mComplement[13]).toInt();
    mComplement[14] = settings.value("promotionalSetComplement", mComplement[14]).toInt();
    mComplement[15] = settings.value("upgradedSetComplement", mComplement[15]).toInt();


    mIsFilterOptionEnabled = settings.value("isFilterOptionEnabled", mIsFilterOptionEnabled).toBool();
    mIsAssaultOptionEnabled = settings.value("isAssaultOptionEnabled", mIsAssaultOptionEnabled).toBool();
    mIsWhiteListEnabled = settings.value("isWhiteListEnabled", mIsWhiteListEnabled).toBool();
    mIsBlackListEnabled = settings.value("isBlackListEnabled", mIsBlackListEnabled).toBool();
    mIsBlockListEnabled = settings.value("isBlockListEnabled", mIsBlockListEnabled).toBool();
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

    settings.setValue("allowHealth1", mIsHealthAllowed[0]);
    settings.setValue("allowHealth2", mIsHealthAllowed[1]);
    settings.setValue("allowHealth3", mIsHealthAllowed[2]);
    settings.setValue("allowHealth4", mIsHealthAllowed[3]);
    settings.setValue("allowHealth5", mIsHealthAllowed[4]);

    settings.setValue("allowAttack0", mIsAttackAllowed[0]);
    settings.setValue("allowAttack1", mIsAttackAllowed[1]);
    settings.setValue("allowAttack2", mIsAttackAllowed[2]);
    settings.setValue("allowAttack3", mIsAttackAllowed[3]);
    settings.setValue("allowAttack4", mIsAttackAllowed[4]);

    settings.setValue("allowDelay0", mIsDelayAllowed[0]);
    settings.setValue("allowDelay1", mIsDelayAllowed[1]);
    settings.setValue("allowDelay2", mIsDelayAllowed[2]);
    settings.setValue("allowDelay3", mIsDelayAllowed[3]);
    settings.setValue("allowDelay4", mIsDelayAllowed[4]);

    settings.setValue("allowFactionBloodthirsty", mIsFactionAllowed[0]);
    settings.setValue("allowFactionImperial", mIsFactionAllowed[1]);
    settings.setValue("allowFactionRaider", mIsFactionAllowed[2]);
    settings.setValue("allowFactionRighteous", mIsFactionAllowed[3]);
    settings.setValue("allowFactionXeno", mIsFactionAllowed[4]);


    QStringList whiteList;
    getWhiteList(whiteList);
    settings.setValue("whiteList", whiteList);

    QStringList blackList;
    getBlackList(blackList);
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
    settings.setValue("flashpointSetComplement", mComplement[12]);
    settings.setValue("rewardSetComplement", mComplement[13]);
    settings.setValue("promotionalSetComplement", mComplement[14]);
    settings.setValue("upgradedSetComplement", mComplement[15]);

    settings.setValue("isFilterOptionEnabled", mIsFilterOptionEnabled);
    settings.setValue("isAssaultOptionEnabled", mIsAssaultOptionEnabled);
    settings.setValue("isWhiteListEnabled", mIsWhiteListEnabled);
    settings.setValue("isBlackListEnabled", mIsBlackListEnabled);
    settings.setValue("isBlockListEnabled", mIsBlockListEnabled);
    settings.setValue("isCompletionEnabled", mIsCompletionEnabled);

    settings.endGroup();
}

void CCardFilterParameters::setDeckBlockage(const CDeck &deck, bool isBlocked)
{
    const TCardList &cards = deck.getCards();
    for (TCardList::const_iterator iCards = cards.begin(); iCards != cards.end(); ++iCards)
    {
        QHash<QString, int>::iterator iBlock = mBlockList.find((*iCards).getName());
        if (isBlocked)
        {
            if (iBlock != mBlockList.end())
            {
                if (iBlock.value() > 0)
                {
                    ++iBlock.value();
                }
            }
            else
            {
                mBlockList.insert((*iCards).getName(), 1);
            }
        }
        else
        {
            if (iBlock != mBlockList.end())
            {
                if (iBlock.value() > 0)
                {
                    --iBlock.value();
                    if (iBlock.value() <= 0)
                    {
                        mBlockList.remove((*iCards).getName());
                    }
                }
            }
        }
    }
}

void CCardFilterParameters::setCardBlackListStatus(const CCard &card, bool isBlack)
{
    QSet<QString>::iterator iBlack = mBlackList.find(card.getName());
    if (iBlack == mBlackList.end() && isBlack)
    {
        setCardWhiteListStatus(card, false);
        mBlackList.insert(card.getName());
    }
    else if (iBlack != mBlackList.end() && !isBlack)
    {
        mBlackList.erase(iBlack);
    }
}

void CCardFilterParameters::setCardWhiteListStatus(const CCard &card, bool isWhite)
{
    QSet<QString>::iterator iWhite = mWhiteList.find(card.getName());
    if (iWhite == mWhiteList.end() && isWhite)
    {
        setCardBlackListStatus(card, false);
        mWhiteList.insert(card.getName());
    }
    else if (iWhite != mWhiteList.end() && !isWhite)
    {
        mWhiteList.erase(iWhite);
    }
}

void CCardFilterParameters::getBlackList(QStringList &blackList) const
{
    for(QSet<QString>::const_iterator i = mBlackList.begin(); i != mBlackList.end(); ++i)
    {
        if (!(*i).isEmpty())
        {
            blackList.push_back((*i));
        }
    }
}

void CCardFilterParameters::getWhiteList(QStringList &whiteList) const
{
    for(QSet<QString>::const_iterator i = mWhiteList.begin(); i != mWhiteList.end(); ++i)
    {
        if (!(*i).isEmpty())
        {
            whiteList.push_back((*i));
        }
    }
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
        case EFlashpointSet: num = qMax(num, mComplement[12]); break;
        case EStandardSet: num = qMax(num, mComplement[0]); break;
        case ERewardSet: num = qMax(num, mComplement[13]); break;
        case EPromotionalSet: num = qMax(num, mComplement[14]); break;
        case EUpgradedSet: num = qMax(num, mComplement[15]); break;
        default: break;
        }
    }

    if (mIsBlackListEnabled)
    {
        if (mBlackList.contains(card.getName()))
        {
            return false;
        }
    }

    if (mIsBlockListEnabled)
    {
        QHash<QString, int>::const_iterator iBlock = mBlockList.find(card.getName());
        if (iBlock != mBlockList.end())
        {
            num = qMax(0, num - iBlock.value());
        }
        if (num < 1)
        {
            return false;
        }
    }

    if (mIsWhiteListEnabled)
    {
        if (mWhiteList.contains(card.getName()))
        {
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

        bool passFaction(true);
        bool passHealth(true);
        bool passAttack(true);
        bool passDelay(true);
        if (mIsAssaultOptionEnabled && card.getType() == EAssaultType)
        {
            switch(card.getFaction())
            {
            case EBloodthirstyFaction: passFaction = mIsFactionAllowed[0]; break;
            case EImperialFaction: passFaction = mIsFactionAllowed[1]; break;
            case ERaiderFaction: passFaction = mIsFactionAllowed[2]; break;
            case ERighteousFaction: passFaction = mIsFactionAllowed[3]; break;
            case EXenoFaction: passFaction = mIsFactionAllowed[4]; break;
            default: break;
            }
            switch (card.getHealth())
            {
            case 1: passHealth = mIsHealthAllowed[0]; break;
            case 2: passHealth = mIsHealthAllowed[1]; break;
            case 3: passHealth = mIsHealthAllowed[2]; break;
            case 4: passHealth = mIsHealthAllowed[3]; break;
            case 5:
            default: passHealth = mIsHealthAllowed[4]; break;
            }
            switch (card.getAttack())
            {
            case 0: passAttack = mIsAttackAllowed[0]; break;
            case 1: passAttack = mIsAttackAllowed[1]; break;
            case 2: passAttack = mIsAttackAllowed[2]; break;
            case 3: passAttack = mIsAttackAllowed[3]; break;
            case 4:
            default: passAttack = mIsAttackAllowed[4]; break;
            }
            switch (card.getDelay())
            {
                case 0: passDelay = mIsDelayAllowed[0]; break;
                case 1: passDelay = mIsDelayAllowed[1]; break;
                case 2: passDelay = mIsDelayAllowed[2]; break;
                case 3: passDelay = mIsDelayAllowed[3]; break;
                case 4: passDelay = mIsDelayAllowed[4]; break;
                default: break;
            }
        }
        return passRarity && passFaction && passHealth && passAttack && passDelay;
    }
    return true;
}

TListedCard CCardFilterParameters::strToListedCard(const QString &str) const
{
    TListedCard card("", -1);
    QStringList curCardTokens = str.split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
    if (curCardTokens.size() > 0)
    {
        int num(-1);
        if (curCardTokens.size() > 1)
        {
            bool ok(true);
            num = curCardTokens.at(1).toInt(&ok);
        }
        card.first = curCardTokens.at(0).trimmed();
        card.second = num;
    }
    return card;
}

QString CCardFilterParameters::listedCardToStr(const TListedCard &card) const
{
    if (card.second > 0)
    {
        return QString("%1 (%2)").arg(card.first).arg(card.second);
    }
    else
    {
        return card.first;
    }
}

