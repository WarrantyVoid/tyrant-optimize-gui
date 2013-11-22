#include "CVersion.h"
#include <QStringList>

const CVersion CVersion::INVALID_VERSION;

CVersion::CVersion()
: mMajor(-1)
, mMinor(-1)
, mPatch(-1)
{

}

CVersion::CVersion(const QString &versionStr)
: mMajor(-1)
, mMinor(-1)
, mPatch(-1)
{
    QStringList versionSplit = versionStr.split(".");
    if (versionSplit.size() == 3)
    {
        bool ok(true);
        mMajor = versionSplit[0].toInt(&ok);
        mMinor = versionSplit[1].toInt(&ok);
        mPatch = versionSplit[2].toInt(&ok);
    }
}

CVersion::CVersion(int major, int minor, int patch)
: mMajor(major)
, mMinor(minor)
, mPatch(patch)
{

}

bool CVersion::operator<(const CVersion &other)
{
    if (mMajor == other.mMajor)
    {
        if (mMinor == other.mMinor)
        {
            return mPatch < other.mPatch;
        }
        else
        {
            return mMinor < other.mMinor;
        }
    }
    else
    {
        return mMajor < other.mMajor;
    }
}

bool CVersion::operator>(const CVersion &other)
{
    if (mMajor == other.mMajor)
    {
        if (mMinor == other.mMinor)
        {
            return mPatch > other.mPatch;
        }
        else
        {
            return mMinor > other.mMinor;
        }
    }
    else
    {
        return mMajor > other.mMajor;
    }
}

bool CVersion::operator==(const CVersion &other)
{
    return mMajor == other.mMajor
        && mMinor == other.mMinor
        && mPatch == other.mPatch;
}

bool CVersion::isValid() const
{
    return mMajor > -1
        && mMinor > -1
        && mPatch > -1;
}

QString CVersion::toString() const
{
    return isValid()
        ? QString("%1.%2.%3").arg(mMajor).arg(mMinor).arg(mPatch)
        : QString("unknown");
}
