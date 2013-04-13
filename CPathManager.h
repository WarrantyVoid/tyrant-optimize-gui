#ifndef CPATHMANAGER_H
#define CPATHMANAGER_H

#include <QString>

class CPathManager
{
public:
    CPathManager();
    static const CPathManager& getPathManager();

public:
    const QString& getAppPath() const;
    const QString& getToolPath() const;
    const QString& getPicturePath() const;
    const QString& getOnlinePicturePath() const;
    const QString& getResourcePicturePath() const;

private:
    QString mSep;
    QString mAppPath;
    QString mToolPath;
    QString mPicturePath;
    QString mOnlinePicturePath;
    QString mResourcePicturePath;

    static CPathManager *PATH_MANAGER;
};

#endif // CPATHMANAGER_H
