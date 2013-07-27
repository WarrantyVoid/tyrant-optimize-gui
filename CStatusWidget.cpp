#include "CStatusWidget.h"
#include "CGlobalConfig.h"

CStatusWidget::CStatusWidget(QWidget *parent)
: QWidget(parent)
, mHorizontalLayout(0)
, mStatusIconLabel(0)
, mStatusTimeLabel(0)
, mUpdateTimer()
, mReferenceTime()
, mMoviePlayer(0)
, mSoundPlayer(0)
{
    mMoviePlayer = new QMovie(this);
    mMoviePlayer->setFileName(":/img/busy.gif");
    mSoundPlayer = Phonon::createPlayer(Phonon::NoCategory, Phonon::MediaSource(":/snd/notify.wav"));

    mHorizontalLayout = new QHBoxLayout(this);
    mHorizontalLayout->setContentsMargins(0, 0, 0, 0);
    mHorizontalLayout->setSpacing(2);

    mStatusIconLabel = new QLabel(this);
    mStatusIconLabel->setFixedSize(16, 16);
    mHorizontalLayout->addWidget(mStatusIconLabel);

    mStatusTimeLabel = new QLabel(this);
    mStatusTimeLabel->setAlignment(Qt::AlignCenter);
    mStatusTimeLabel->setText("00:00:00");
    mHorizontalLayout->addWidget(mStatusTimeLabel);

    connect(&mUpdateTimer, SIGNAL(timeout()),
            this, SLOT(updateElapsedTime()));
}

CStatusWidget::~CStatusWidget()
{
    if (mSoundPlayer)
    {
        delete mSoundPlayer;
        mSoundPlayer = 0;
    }
}

QString CStatusWidget::getStatusTime() const
{
    return mStatusTimeLabel->text();
}

void CStatusWidget::setStatus(EStatusValue status)
{
    bool isSoundEnabled = !CGlobalConfig::getCfg().isSoundMuted();
    switch(status)
    {
    case EStatusUnknown:
        mStatusIconLabel->clear();
        updateElapsedTime();
        mUpdateTimer.stop();
        break;
    case EStatusBusy:
        mStatusIconLabel->setMovie(mMoviePlayer);
        mMoviePlayer->start();
        mReferenceTime.start();
        updateElapsedTime();
        mUpdateTimer.start(500);
        break;
    case EStatusResultSuccess:
        mStatusIconLabel->setPixmap(QPixmap(":/img/success"));
        updateElapsedTime();
        if (isSoundEnabled)
        {
            mSoundPlayer->seek(0);
            mSoundPlayer->play();
        }
        mUpdateTimer.stop();
        break;
    case EStatusResultFailure:
        mStatusIconLabel->setPixmap(QPixmap(":/img/warning"));
        updateElapsedTime();
        if (isSoundEnabled)
        {
            mSoundPlayer->seek(0);
            mSoundPlayer->play();
        }
        mUpdateTimer.stop();
        break;
    }
}

void CStatusWidget::updateElapsedTime()
{
    int elapsed = mReferenceTime.elapsed() / 1000;
    mStatusTimeLabel->setText(QString("%1:%2:%3")
        .arg((elapsed / 3600) % 24, 2, 10, QChar('0'))
        .arg((elapsed / 60  ) % 60, 2, 10, QChar('0'))
        .arg(elapsed % 60, 2, 10, QChar('0')));
}
