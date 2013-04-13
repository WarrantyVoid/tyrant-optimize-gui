#include "CPathManager.h"
#include <QApplication>
#include <QDir>

CPathManager* CPathManager::PATH_MANAGER = 0;

CPathManager::CPathManager()
: mSep(QDir::separator())
, mAppPath(QApplication::applicationDirPath() + mSep)
, mToolPath(mAppPath + "tyrant_optimize" + mSep)
, mPicturePath(mAppPath + "cards" + mSep)
, mOnlinePicturePath("http://cdn.tyrantonline.com/warhawk/images/")
, mResourcePicturePath(":/img/tyrant/")
{
}

const CPathManager& CPathManager::getPathManager()
{
    if (!PATH_MANAGER)
    {
        PATH_MANAGER = new CPathManager();
    }
    return *PATH_MANAGER;
}

const QString& CPathManager::getAppPath() const
{
    return mAppPath;
}

const QString& CPathManager::getToolPath() const
{
    return mToolPath;
}

const QString& CPathManager::getPicturePath() const
{
    return mPicturePath;
}

const QString& CPathManager::getOnlinePicturePath() const
{
    return mOnlinePicturePath;
}

const QString& CPathManager::getResourcePicturePath() const
{
    return mResourcePicturePath;
}


