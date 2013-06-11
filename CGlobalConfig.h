#ifndef CGLOBALCONFIG_H
#define CGLOBALCONFIG_H

#include <QString>
#include <QSettings>

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

    void setLastDir(const QString& dir);
    void setCardShadingEnabled(bool enabled);
    const QString& getLastDir() const;
    bool isCardShadingEnabled() const;

private:
    QString mSep;
    QString mAppPath;
    QString mToolPath;
    QString mPicturePath;
    QString mOnlinePicturePath;
    QString mResourcePicturePath;

    QString mLastDir;
    bool mIsCardShadingEnabled;

    static CGlobalConfig *GLOBAL_CONFIG;
};

#endif // CGLOBALCONFIG_H