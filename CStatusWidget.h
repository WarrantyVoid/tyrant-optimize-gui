#ifndef CSTATUSWIDGET_H
#define CSTATUSWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QMovie>
#include <phonon/phonon>

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
    Phonon::MediaObject *mSoundPlayer;
};

#endif // CSTATUSWIDGET_H
