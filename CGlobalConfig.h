#ifndef CGLOBALCONFIG_H
#define CGLOBALCONFIG_H

#include <QString>
#include <QSettings>
#include <QAction>

class CGlobalConfig
{
public:
    CGlobalConfig();
    static CGlobalConfig& getCfg();

public:
    void load(QSettings &settings);
    void save(QSettings &settings);

    const QString& getAppPath() const;
    const QString& getToolPath() const;
    const QString& getPicturePath() const;
    const QString& getOnlinePicturePath() const;
    const QString& getResourcePicturePath() const;
    const QString& getCustomPicturePath() const;

    void setLastDir(const QString& dir);
    void setCardShadingEnabled(bool enabled);
    void setBlackLabellingEnabled(bool enabled);
    void setWhiteLabellingEnabled(bool enabled);
    void setSoundMuted(bool muted);
    const QString& getLastDir() const;
    bool isCardShadingEnabled() const;
    bool isBlackLabellingEnabled() const;
    bool isWhiteLabellingEnabled() const;
    bool isSoundMuted() const;

private:
    QString mSep;
    QString mAppPath;
    QString mToolPath;
    QString mPicturePath;
    QString mOnlinePicturePath;
    QString mResourcePicturePath;
    QString mCustomPicturePath;

    QString mLastDir;
    bool mIsCardShadingEnabled;
    bool mIsBlackLabellingEnabled;
    bool mIsWhiteLabellingEnabled;
    bool mIsSoundMuted;

    static CGlobalConfig *GLOBAL_CONFIG;
};

#endif // CGLOBALCONFIG_H
