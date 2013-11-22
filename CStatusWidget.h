#ifndef CSTATUSWIDGET_H
#define CSTATUSWIDGET_H

#include <QMediaPlayer>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QMovie>

enum EStatusValue
{
    EStatusUnknown,
    EStatusBusy,
    EStatusResultSuccess,
    EStatusResultFailure
};

class CStatusWidget : public QWidget
{

   Q_OBJECT

public:
    CStatusWidget(QWidget* parent = 0);
    virtual ~CStatusWidget();

public:
    QString getStatusTime() const;

public slots:
    void setStatus(EStatusValue status);

protected slots:
    void updateElapsedTime();

private:
    QHBoxLayout *mHorizontalLayout;
    QLabel *mStatusIconLabel;
    QLabel *mStatusTimeLabel;

    QTimer mUpdateTimer;
    QTime mReferenceTime;
    QMovie *mMoviePlayer;
    QMediaPlayer *mSoundPlayer;
};

#endif // CSTATUSWIDGET_H
