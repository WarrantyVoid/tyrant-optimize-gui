#include "CWinLabel.h"
#include "CGlobalConfig.h"
#include "CCardLabel.h"
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
    if (ev && CCardLabel::isCardLabelDropData(ev->mimeData()))
    {
        QString textData = ev->mimeData()->text().toLatin1();
        QStringList valueList = textData.split(QRegExp("=|;"));
        if (valueList.size() == 4)
        {
            bool ok1(true);
            bool ok2(true);
            valueList.at(1).toInt(&ok1);
            valueList.at(3).toInt(&ok2);
            if (ok1 && ok2)
            {
                ev->accept();
                emit unitDropped();
            }
        }
    }
}

void CWinLabel::dragMoveEvent(QDragMoveEvent *ev)
{
    if (ev && CCardLabel::isCardLabelDropData(ev->mimeData()))
    {
        ev->accept();
    }
}

void CWinLabel::dragEnterEvent(QDragEnterEvent *ev)
{
    if (ev && CCardLabel::isCardLabelDropData(ev->mimeData()))
    {
        ev->acceptProposedAction();
    }
}
