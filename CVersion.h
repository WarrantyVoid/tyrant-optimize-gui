#ifndef CVERSION_H
#define CVERSION_H

#include <QString>

class CVersion
{
public:
    CVersion();
    CVersion(const QString &versionStr);
    CVersion(int major, int minor, int patch);

public:
    bool operator<(const CVersion &other);
    bool operator>(const CVersion &other);
    bool operator==(const CVersion &other);

 public:
    bool isValid() const;
    QString toString() const;

public:
    static const CVersion INVALID_VERSION;

private:
    int mMajor;
    int mMinor;
    int mPatch;
};

#endif // CVERSION_H
