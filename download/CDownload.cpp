#include "CDownload.h"
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QFile>

CDownload::CDownload(const QString &urlName, const QString &fileName, bool hashCheck)
: mUrlName(urlName)
, mFileName(fileName)
, mNetReply(0)
, mState(EDownloadNotStarted)
, mProgress(0)
, mIsHashChecked(hashCheck)
{

}

CDownload::~CDownload()
{
    if (mNetReply)
    {
        disconnect(
            mNetReply, SIGNAL(finished()),
            this, SLOT(downloadFinished()));

        disconnect(
            mNetReply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(downloadError(QNetworkReply::NetworkError)));
        mNetReply->abort();
        delete mNetReply;
        mNetReply = 0;
    }
}

void CDownload::startDownload(QNetworkAccessManager* netManager)
{
    if (mState == EDownloadNotStarted)
    {
        mState = EDownloadFailed;
        if (netManager)
        {
            QNetworkAccessManager::NetworkAccessibility access = netManager->networkAccessible();
            if (access != QNetworkAccessManager::NotAccessible)
            {
                mNetReply = netManager->get(QNetworkRequest(QUrl(mUrlName)));
                if (mNetReply)
                {
                    connect(
                        mNetReply, SIGNAL(finished()),
                        this, SLOT(downloadFinished()));

                    connect(
                        mNetReply, SIGNAL(error(QNetworkReply::NetworkError)),
                        this, SLOT(downloadError(QNetworkReply::NetworkError)));
                    //connect(
                    //    mNetReply, SIGNAL(downloadProgress(qint64, qint64)),
                    //    this, SLOT(downloadProgress(qint64, qint64)));
                    mState = EDownloadStarted;
                }
            }
        }
    }
}

void CDownload::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    int progress = (bytesReceived * 100) / bytesTotal;
    if (progress != mProgress)
    {
        mProgress = progress;
        emit downloadProgress(mProgress);
    }
}

void CDownload::downloadError(QNetworkReply::NetworkError /*error*/)
{
    mState = EDownloadFailed;
}

void CDownload::downloadFinished()
{
    mState = EDownloadFailed;
    if (mNetReply)
    {
        if (mNetReply->isFinished() && mNetReply->error() == QNetworkReply::NoError)
        {
            QByteArray downData = mNetReply->readAll();
            QFile file(mFileName);
            bool doReplace = true;

            if (mIsHashChecked)
            {
                QCryptographicHash downHash(QCryptographicHash::Sha1);
                downHash.addData(downData);
                QCryptographicHash fileHash(QCryptographicHash::Sha1);
                if (file.open(QIODevice::ReadOnly))
                {
                    fileHash.addData(file.readAll());
                }
                file.close();
                doReplace = (QString(downHash.result().toHex()).compare(QString(fileHash.result().toHex())) != 0);
            }

            if (!downData.isEmpty() && !downData.isNull())
            {
                mState = EDownloadSuccessful;
                if (doReplace)
                {
                    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate))
                    {
                        mState = EDownloadStored;
                        file.write(downData);
                        file.close();
                    }
                    else
                    {
                        mState = EDownloadStoreFailed;
                    }
                }
            }
        }
        mNetReply->deleteLater();
        mNetReply = 0;
    }
    emit downloadHandled();
}
