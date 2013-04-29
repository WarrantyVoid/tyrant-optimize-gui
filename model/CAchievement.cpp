#include "CAchievement.h"

const CAchievement CAchievement::INVALID_ACHIEVEMENT;

CAchievement::CAchievement()
: mId(0u)
, mName("")
, mDescription("")
{
}

void CAchievement::setId(unsigned int id)
{
    mId = id;
}

void CAchievement::setName(const QString &name)
{
    mName = name;
}

void CAchievement::setDescription(const QString &description)
{
    mDescription = description;
}

unsigned int CAchievement::getId() const
{
    return mId;
}

QString CAchievement::getName() const
{
    return mName;
}

QString CAchievement::getDescription() const
{
    return mDescription;
}

bool CAchievement::isValid() const
{
    return mId != 0 && !mName.isEmpty();
}
