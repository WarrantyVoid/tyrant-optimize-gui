#ifndef CCARD_H
#define CCARD_H

#include <QString>
#include <QList>

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
    EStandardSet = 1000,
    ERewardSet = 5000,
    EPromotionalSet = 5001,
    EUpgradedSet = 5002,
    ESpecialSet = 9000
};

class CCardSkill
{
public:
    CCardSkill(const QString &id, int x = -1, int y = - 1, bool all = false, unsigned int flags = SKILL_FLAG_NONE);

public:
    QString getId() const;
    int getX() const;
    ECardFaction getTargetingFaction() const;
    bool isTargetingAll() const;
    unsigned int getFlags() const;

public:
    static const unsigned int SKILL_FLAG_NONE = 0x0;
    static const unsigned int SKILL_FLAG_ON_ATTACKED = 0x1;
    static const unsigned int SKILL_FLAG_ON_KILL = 0x2;
    static const unsigned int SKILL_FLAG_ON_PLAY = 0x4;
    static const unsigned int SKILL_FLAG_ON_DEATH = 0x8;

private:
    QString mId;
    int mX;
    int mY;
    bool mTargetsAll;
    unsigned int mFlags;
};

class CCard
{
public:
    CCard();
    CCard(unsigned int id, const QString &name, const QString &picture, ECardRarity rarity, ECardFaction faction, ECardSet set);

public:
    void setId(unsigned int id);
    void setName(const QString &name);
    void setPicture(const QString &picture);
    void setRarity(ECardRarity rarity);
    void setFaction(ECardFaction type);
    void setSet(ECardSet set);
    void clearSkills();
    void addSkill(const QString &str, int x = -1, int y = - 1, bool all = false, unsigned int flags = CCardSkill::SKILL_FLAG_NONE);
    void setUpgradeLevel(int level);
    void setDelay(int delay);
    void setUnique(bool isUnique);
    void setAttack(int attack);
    void setHealth(int health);

    unsigned int getId() const;
    QString getName() const;
    QString getPicture() const;
    ECardRarity getRarity() const;
    ECardFaction getFaction() const;
    ECardType getType() const;
    ECardSet getSet() const;
    const QList<CCardSkill>& getSkills() const;
    int getUpgradeLevel() const;
    int getDelay() const;
    bool isUnique() const;
    int getAttack() const;
    bool hasAttack() const;
    int getHealth() const;
    bool hasHealth() const;

    bool isValid() const;

public:
    static const CCard INVALID_CARD;
    static const int UPGRADE_LEVEL_NONE;

private:
    unsigned int mId;
    QString mName;
    QString mPicture;
    ECardRarity mRarity;
    ECardFaction mFaction;
    ECardSet mSet;
    QList<CCardSkill> mSkills;
    int mUpgradeLevel;
    int mDelay;
    int mAttack;
    int mHealth;
    bool mIsUnique;
};

#endif // CCARD_H
