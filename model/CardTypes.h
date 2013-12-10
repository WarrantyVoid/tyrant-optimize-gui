#ifndef CARDTYPES_H
#define CARDTYPES_H

enum ECardRarity
{
    EUnknownRarity = 0,
    ECommonRarity = 1,
    EUncommonRarity = 2,
    ERareRarity = 3,
    ELegendaryRarity = 4
};

enum ECardType
{
    EUnknownType = 0,
    EAssaultType = 1,
    ECommanderType = 1000,
    EStructureType = 2000,
    EActionType = 3000

};

enum ECardFaction
{
    EUnknownFaction = 0,
    EImperialFaction = 1,
    EBloodthirstyFaction = 3,
    EXenoFaction = 4,
    ERighteousFaction = 8,
    ERaiderFaction = 9,
    ENoFaction = 0xff
};

enum ECardSet
{
    EUnknownSet = 0,
    EEnclaveSet = 1,
    ENexusSet = 2,
    EBlightSet = 3,
    EPuritySet = 4,
    EHomeWorldSet = 5,
    EPhobosSet = 6,
    EPhobosAftermathSet = 7,
    EAwakeningSet = 8,
    ETerminusSet = 9,
    EOccupationSet = 10,
    EWorldshipSet = 11,
    EFlashpointSet = 12,
    EStandardSet = 1000,
    ERewardSet = 5000,
    EPromotionalSet = 5001,
    EUpgradedSet = 5002,
    ESpecialSet = 9000
};


#endif // CARDTYPES_H
