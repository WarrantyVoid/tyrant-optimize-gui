#include "CGlobalConfig.h"
#include <QApplication>
#include <QDir>

CGlobalConfig* CGlobalConfig::GLOBAL_CONFIG = 0;

CGlobalConfig::CGlobalConfig()
: mSep(QDir::separator())
, mAppPath(QApplication::applicationDirPath() + mSep)
, mToolPath(mAppPath + "tyrant_optimize" + mSep)
, mPicturePath(mAppPath + "cards" + mSep)
, mOnlinePicturePath("http://cdn.tyrantonline.com/warhawk/images/")
, mResourcePicturePath(":/img/tyrant/")
, mCustomPicturePath(mAppPath + "custom" + mSep)
, mLastDir(QApplication::applicationDirPath())
, mIsCardShadingEnabled(true)
, mIsBlackLabellingEnabled(true)
, mIsWhiteLabellingEnabled(true)
, mIsSoundMuted(false)
, mIsDeckActionButtonEnabled(true)
{
}

CGlobalConfig& CGlobalConfig::getCfg()
{
    if (!GLOBAL_CONFIG)
    {
        GLOBAL_CONFIG = new CGlobalConfig();
    }
    return *GLOBAL_CONFIG;
}

void CGlobalConfig::load(QSettings &settings)
{
    mLastDir = settings.value("paths/lastDir", mLastDir).toString();
    mIsCardShadingEnabled = settings.value("view/shadingEnabled", mIsCardShadingEnabled).toBool();
    mIsBlackLabellingEnabled = settings.value("view/blackLabellingEnabled", mIsBlackLabellingEnabled).toBool();
    mIsWhiteLabellingEnabled = settings.value("view/whiteLabellingEnabled", mIsWhiteLabellingEnabled).toBool();
    mIsSoundMuted = settings.value("general/muteSound", mIsSoundMuted).toBool();
    mIsDeckActionButtonEnabled = settings.value("general/deckActionButtons", mIsDeckActionButtonEnabled).toBool();
}

void CGlobalConfig::save(QSettings &settings)
{
    settings.setValue("paths/lastDir", mLastDir);
    settings.setValue("view/shadingEnabled", mIsCardShadingEnabled);
    settings.setValue("view/blackLabellingEnabled", mIsBlackLabellingEnabled);
    settings.setValue("view/whiteLabellingEnabled", mIsWhiteLabellingEnabled);
    settings.setValue("general/muteSound", mIsSoundMuted);
    settings.setValue("general/deckActionButtons", mIsDeckActionButtonEnabled);
}

const QString& CGlobalConfig::getAppPath() const
{
    return mAppPath;
}

const QString& CGlobalConfig::getToolPath() const
{
    return mToolPath;
}

const QString& CGlobalConfig::getPicturePath() const
{
    return mPicturePath;
}

const QString& CGlobalConfig::getOnlinePicturePath() const
{
    return mOnlinePicturePath;
}

const QString& CGlobalConfig::getResourcePicturePath() const
{
    return mResourcePicturePath;
}

const QString& CGlobalConfig::getCustomPicturePath() const
{
    return mCustomPicturePath;
}

void CGlobalConfig::setLastDir(const QString& dir)
{
    mLastDir = dir;
}

void CGlobalConfig::setCardShadingEnabled(bool enabled)
{
    mIsCardShadingEnabled = enabled;
}

void CGlobalConfig::setBlackLabellingEnabled(bool enabled)
{
    mIsBlackLabellingEnabled = enabled;
}

void CGlobalConfig::setWhiteLabellingEnabled(bool enabled)
{
    mIsWhiteLabellingEnabled = enabled;
}

void CGlobalConfig::setSoundMuted(bool muted)
{
    mIsSoundMuted = muted;
}

void CGlobalConfig::setDeckActionButtonEnabled(bool enabled)
{
    mIsDeckActionButtonEnabled = enabled;
}

const QString& CGlobalConfig::getLastDir() const
{
    return mLastDir;
}

bool CGlobalConfig::isCardShadingEnabled() const
{
    return mIsCardShadingEnabled;
}

bool CGlobalConfig::isBlackLabellingEnabled() const
{
    return mIsBlackLabellingEnabled;
}

bool CGlobalConfig::isWhiteLabellingEnabled() const
{
    return mIsWhiteLabellingEnabled;
}

bool CGlobalConfig::isSoundMuted() const
{
    return mIsSoundMuted;
}

bool CGlobalConfig::isDeckActionButtonEnabled() const
{
    return mIsDeckActionButtonEnabled;
}
