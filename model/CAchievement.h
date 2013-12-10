#ifndef CACHIEVEMENT_H
#define CACHIEVEMENT_H

#include <QString>

typedef unsigned int TAchievementId;

class CAchievement
{
public:
    CAchievement();

public:
    void setId(TAchievementId id);
    void setName(const QString &name);
    void setDescription(const QString &description);

    TAchievementId getId() const;
    QString getName() const;
    QString getDescription() const;

    bool isValid() const;

public:
    static const CAchievement INVALID_ACHIEVEMENT;

private:
    TAchievementId mId;
    QString mName;
    QString mDescription;
};

#endif // CACHIEVEMENT_H
