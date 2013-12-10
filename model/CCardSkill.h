#ifndef CCARDSKILL_H
#define CCARDSKILL_H

#include <QList>
#include <QString>
#include "CardTypes.h"

enum ECardSkillFlag
{
    ECardSkillFlagNone = 0x0,
    ECardSkillFlagOnAttacked = 0x1,
    ECardSkillFlagOnKill = 0x2,
    ECardSkillFlagOnPlay = 0x4,
    ECardSkillFlagOnDeath = 0x8
};
Q_DECLARE_FLAGS(ECardSkillFlags, ECardSkillFlag)

class CCardSkill;
typedef QList<CCardSkill> TSkillList;

class CCardSkill
{
public:
    CCardSkill();
    CCardSkill(const QString &id, int x = -1, int y = - 1, bool all = false, ECardSkillFlags flags = ECardSkillFlagNone);

public:
    QString getId() const;
    int getX() const;
    ECardFaction getTargetingFaction() const;
    bool isTargetingAll() const;
    ECardSkillFlags getFlags() const;

    bool isValid() const;

public:
    static const CCardSkill INVALID_CARD_SKILL;

private:
    QString mId;
    int mX;
    int mY;
    bool mTargetsAll;
    ECardSkillFlags mFlags;
};

#endif // CCARDSKILL_H
