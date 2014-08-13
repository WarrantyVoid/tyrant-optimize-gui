#ifndef CCARD_H
#define CCARD_H

#include <QString>
#include <QList>
#include "CardTypes.h"
#include "CCardSkill.h"

class CCard;
typedef QList<CCard> TCardList;
typedef unsigned int TCardId;

class CCard
{
public:
    CCard();

public:
    void setId(TCardId id);
    void setName(const QString &name);
    void setPicture(const QString &picture);
    void setRarity(ECardRarity rarity);
    void setFaction(ECardFaction type);
    void setSet(ECardSet set);
    void clearSkills();
    void addSkill(const CCardSkill& skill);
    void setUpgradeLevel(ECardUpgradeLevel level);
    void setDelay(int delay);
    void setUnique(bool isUnique);
    void setAttack(int attack);
    void setHealth(int health);

    TCardId getId() const;
    QString getName() const;
    QString getBaseName() const;
    QString getUpgradedName() const;
    QString getLwCsName() const;
    QString getPicture() const;
    ECardRarity getRarity() const;
    ECardFaction getFaction() const;
    ECardType getType() const;
    ECardSet getSet() const;
    const TSkillList& getSkills() const;
    ECardUpgradeLevel getUpgradeLevel() const;
    int getDelay() const;
    bool isUnique() const;
    int getAttack() const;
    bool hasAttack() const;
    int getHealth() const;
    bool hasHealth() const;

    bool isValid() const;

public:
    static const CCard INVALID_CARD;

private:
    TCardId mId;
    QString mName;
    QString mLwCsName;
    QString mPicture;
    ECardRarity mRarity;
    ECardFaction mFaction;
    ECardSet mSet;
    QList<CCardSkill> mSkills;
    ECardUpgradeLevel mUpgradeLevel;
    int mDelay;
    int mAttack;
    int mHealth;
    bool mIsUnique;
};

#endif // CCARD_H
