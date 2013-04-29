#ifndef CACHIEVEMENT_H
#define CACHIEVEMENT_H

#include <QString>

class CAchievement
{
public:
    CAchievement();

public:
    void setId(unsigned int id);
    void setName(const QString &name);
    void setDescription(const QString &description);

    unsigned int getId() const;
    QString getName() const;
    QString getDescription() const;

    bool isValid() const;

public:
    static const CAchievement INVALID_ACHIEVEMENT;

private:
    unsigned int mId;
    QString mName;
    QString mDescription;
};

#endif // CACHIEVEMENT_H
