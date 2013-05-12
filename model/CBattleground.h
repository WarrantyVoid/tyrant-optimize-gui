#ifndef CBATTLEGROUND_H
#define CBATTLEGROUND_H

#include <QString>

class CBattleground
{
public:
    CBattleground();

public:
    void setId(unsigned int id);
    void setName(const QString &name);
    void setDescription(const QString &description);
    void setPicture(const QString &picture);

    unsigned int getId() const;
    QString getName() const;
    QString getDescription() const;
    QString getPicture() const;

    bool isValid() const;

public:
    static const CBattleground INVALID_BATTLEGROUND;

private:
    unsigned int mId;
    QString mName;
    QString mDescription;
    QString mPicture;
};

#endif // CBATTLEGROUND_H
