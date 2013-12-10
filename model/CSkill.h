#ifndef CSKILL_H
#define CSKILL_H

#include <QString>
#include "CCardSkill.h"

class CSkill
{
public:
    CSkill();

public:
    void setId(const QString& id);
    void setName(const QString& name);
    void setPicture(const QString& picture);
    void setDescription(const QString& description);

    QString getId() const;
    QString getName() const;
    QString getPicture() const;
    QString getDescription() const;

    QString makeSignature(const CCardSkill& skillParam, bool linkSummon=false) const;
    bool isSummon() const;
    bool isValid() const;

public:
    static const CSkill INVALID_SKILL;

private:
    QString mId;
    QString mName;
    QString mPicture;
    QString mDescription;
};

#endif // CSKILL_H
