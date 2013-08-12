#include "CWinLabel.h"
#include "CGlobalConfig.h"
#include <QPaintEvent>
#include <QPainter>

CWinLabel::CWinLabel(QWidget *parent)
: QLabel(parent)
, mWintext("")
{
}

void CWinLabel::setWinText(const SOptimizationStatus &status, EOptimizationMode mode)
{
    const QString &rcPath = CGlobalConfig::getCfg().getResourcePicturePath();
    switch (mode)
    {
    case EOptimizeWin:
        setToolTip(QString("Chance to win: %1%").arg(status.chanceWin, -1, 'f', 3));
        mWintext = QString("%1%").arg(status.chanceWin, -1, 'f', 3);
        mWinPic = rcPath + "WarWonIcon.png";
        break;
    case EOptimizeDefense:
        setToolTip(QString("Chance to win or draw: %1%").arg(status.chanceWin + status.chanceStall, -1, 'f', 3));
        mWintext = QString("%1%").arg(status.chanceWin + status.chanceStall, -1, 'f', 3);
        mWinPic = rcPath + "WarDefIcon.png";
        break;
    case EOptimizeRaid:
        setToolTip(QString("Average raid damage: %1").arg(status.avRaidDmg, -1, 'f', 2));
        mWintext = QString("%1").arg(status.avRaidDmg, -1, 'f', 2);
        mWinPic = rcPath + "RaidIcon.png";
        break;
    case EOptimizeAchievement:
        setToolTip(QString("Chance to get achievement: %1%").arg(status.chanceWin, -1, 'f', 3));
        mWintext = QString("%1%").arg(status.chanceWin, -1, 'f', 3);
        mWinPic = rcPath + "AchievementBadgeIcon.png";
        break;
    }
    update();
}

void CWinLabel::paintEvent(QPaintEvent *ev)
{
    QLabel::paintEvent(ev);
    int iconW = 24;
    int iconH = 24;

    int textW = ev->rect().width() - 4;
    int textH = 24;

    QPainter painter(this);
    painter.drawPixmap((width() - iconW) / 2, (height() - iconH) / 2 - 16, mWinPic);
    painter.drawText(QRect(2, (height() - iconW) / 2 + 8, textW, textH), Qt::AlignCenter, mWintext);
}

void CWinLabel::dropEvent(QDropEvent *ev)
{
    QString data = ev->mimeData()->text().toLatin1();
    if (data.length() < 50)
    {
        QStringList valueList = data.split("=");
        if (valueList.size() == 2)
        {
            bool ok(true);
            valueList.at(1).toInt(&ok);
            if (ok)
            {
                ev->accept();
                emit unitDropped();
            }
        }
    }
}

void CWinLabel::dragMoveEvent(QDragMoveEvent *ev)
{
    ev->accept();
}

void CWinLabel::dragEnterEvent(QDragEnterEvent *ev)
{
    ev->acceptProposedAction();
}
