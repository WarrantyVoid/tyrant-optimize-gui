#ifndef CDOWNLOAD_H
#define CDOWNLOAD_H

#include <QObject>
#include <QNetworkReply>

class QNetworkAccessManager;
class QNetworkReply;

enum EDownloadState
{
    EDownloadNotStarted,
    EDownloadStarted,
    EDownloadFailed,
    EDownloadSuccessful,
    EDownloadStored,
    EDownloadStoreFailed
};

class CDownload : public QObject
{
    Q_OBJECT
public:
    CDownload(const QString &urlName, const QString &fileName, bool hashCheck = false);
    virtual ~CDownload();

public:
    void startDownload(QNetworkAccessManager* netManager);    
    QString urlName() const { return mUrlName; }
    QString fileName() const { return mFileName; }
    EDownloadState state() const { return mState; }

signals:
    void downloadProgress(int percent);
    void downloadHandled();

protected slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadError(QNetworkReply::NetworkError error);    
    void downloadFinished();

private:
    QString mUrlName;
    QString mFileName;
    QNetworkReply *mNetReply;
    EDownloadState mState;
    int mProgress;
    bool mIsHashChecked;    
};

#endif // CDOWNLOAD_H
