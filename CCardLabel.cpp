#include "CCardLabel.h"
#include "CCardLabelNexus.h"
#include "CGlobalConfig.h"
#include "model/CCardTable.h"
#include <QApplication>
#include <QPaintEvent>
#include <QPainter>

CCardLabel::CCardLabel(QWidget *parent)
: QLabel(parent)
, mCards(CCardTable::getCardTable())
, mTitleIcon()
, mLastLeftClickPos(0)
, mLockButton(0)
{
    CCardLabelNexus::getCardLabelNexus().registerCardLabel(this);
    setScaledContents(true);   
}

CCardLabel::~CCardLabel()
{
    CCardLabelNexus::getCardLabelNexus().unregisterCardLabel(this);
    if (mLastLeftClickPos)
    {
        delete mLastLeftClickPos;
        mLastLeftClickPos = 0;
    }
}

void CCardLabel::setLockEnabled(bool enabled)
{
    if (enabled && mLockButton == 0)
    {
        mLockButton = new QPushButton(this);

        QIcon icon;
        icon.addPixmap(QPixmap(":/img/unlocked.png"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/img/locked.png"), QIcon::Normal, QIcon::On);
        mLockButton->setIcon(icon);
        mLockButton->setCheckable(true);
        mLockButton->setVisible(false);
    }
    else if (mLockButton != 0)
    {
        delete mLockButton;
        mLockButton = 0;
    }
    setMouseTracking(enabled);
}

void CCardLabel::setLocked(bool locked)
{
    if (mLockButton)
    {
        mLockButton->setChecked(locked);
        mLockButton->setVisible(locked);
    }
}

bool CCardLabel::isLocked() const
{
    return mLockButton && mLockButton->isChecked();
}

void CCardLabel::setCard(const CCard& card)
{
    const CGlobalConfig &cfg = CGlobalConfig::getCfg();
    mCard = card;
    mTitleIcon = QPixmap();
    switch(card.getType())
    {
    case EAssaultType:
        switch(card.getRarity())
        {
        case ECommonRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "AssaultRegIcon.png"); break;
        case EUncommonRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "AssaultSilverIcon.png"); break;
        case ERareRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "AssaultGoldIcon.png"); break;
        case ELegendaryRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "AssaultLegendaryIcon.png"); break;
        default: break;
        }
        break;
    case ECommanderType:
        switch(card.getRarity())
        {
        case ECommonRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "CommanderRegIcon.png"); break;
        case EUncommonRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "CommanderSilverIcon.png"); break;
        case ERareRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "CommanderGoldIcon.png"); break;
        case ELegendaryRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "CommanderLegendaryIcon.png"); break;
        default: break;
        }
        break;
    case EStructureType:
        switch(card.getRarity())
        {
        case ECommonRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "StructureRegIcon.png"); break;
        case EUncommonRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "StructureSilverIcon.png"); break;
        case ERareRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "StructureGoldIcon.png"); break;
        case ELegendaryRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "StructureLegendaryIcon.png"); break;
        default: break;
        }
        break;
    case EActionType:
        switch(card.getRarity())
        {
        case ECommonRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "ActionRegIcon.png"); break;
        case EUncommonRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "ActionSilverIcon.png"); break;
        case ERareRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "ActionGoldIcon.png"); break;
        case ELegendaryRarity: mTitleIcon.load(cfg.getResourcePicturePath() + "ActionLegendaryIcon.png"); break;
        default: break;
        }
        break;
    default:
        break;

    }
    QPixmap cardImg;
    switch(card.getFaction())
    {
    case EImperialFaction: cardImg.load(cfg.getResourcePicturePath() + "CardImperialStyled.png"); break;
    case EBloodthirstyFaction: cardImg.load(cfg.getResourcePicturePath() + "CardBloodthirstyStyle.png"); break;
    case EXenoFaction: cardImg.load(cfg.getResourcePicturePath() + "CardXenoStyle.png"); break;
    case ERighteousFaction: cardImg.load(cfg.getResourcePicturePath() + "CardRighteousStyle.png"); break;
    case ERaiderFaction: cardImg.load(cfg.getResourcePicturePath() + "CardRaiderStyle.png"); break;
    case ENoFaction: cardImg.load(cfg.getResourcePicturePath() + "CardBasic.png"); break;
    default: cardImg.load(cfg.getResourcePicturePath() + "CardBack.png"); break;
    }

    QPixmap unitImg;
    if (!unitImg.load(cfg.getPicturePath() + card.getPicture()))
    {
        // workaround for invalid file extension
        unitImg.load(cfg.getPicturePath() + card.getPicture(), "png");
    }
    QPainter painter(&cardImg);
    painter.drawPixmap(5, 24, 150, 120, unitImg.copy(0, 15, unitImg.width(), unitImg.height() - 30));
    painter.end();
    QLabel::setPixmap(cardImg);

    const QList<CCardSkill> &skills = mCard.getSkills();
    QStringList skillDescr;
    skillDescr.append(QString("<b>%1</b><br>").arg(mCard.getName()));
    QString subTitleStr = mCard.isUnique() ? "Unique " : "";
    switch(mCard.getFaction())
    {
    case EImperialFaction: subTitleStr += "Imperial"; break;
    case EBloodthirstyFaction: subTitleStr += "Bloodthirsty"; break;
    case EXenoFaction: subTitleStr += "Xeno"; break;
    case ERighteousFaction: subTitleStr += "Righteous"; break;
    case ERaiderFaction: subTitleStr += "Raider"; break;
    default: break;
    }
    skillDescr.append(QString("<i>%1</i><br>").arg(subTitleStr));
    for (int i = 0; i < skills.size(); ++i)
    {
        const CSkill& curSkill = mCards.getSkillForId(skills.at(i).getId());
        if (curSkill.isValid())
        {
            skillDescr.append(QString("<img src='" + cfg.getResourcePicturePath() + "%1.png'/> %2<br>")
                .arg(curSkill.getPicture())
                .arg(curSkill.makeSignature(skills.at(i))));
        }
    }
    QLabel::setToolTip(skillDescr.join(""));
}

const CCard& CCardLabel::getCard() const
{
    return mCard;
}

void CCardLabel::paintEvent(QPaintEvent *ev)
{
    QRectF titleRect(translatePoint(5, 2), translatePoint(155, 25));
    QRectF imgRect(translatePoint(5, 24), translatePoint(155, 144));
    QRectF blackRect(translatePoint(6, 24 + 40), translatePoint(156, 24 + 80));
    QRectF setRect(translatePoint(155 - 2 - 32, 144 + 8), translatePoint(155 - 2, 144 + 8 + 32));
    QRectF delayRect(translatePoint(155 - 40, 24), translatePoint(155,  24 + 40));
    QRectF attackRect(translatePoint(5 + 2, 215 - 20), translatePoint(5 + 2 + 20, 215));
    QRectF healthRect(translatePoint(155 - 2 - 20, 215 - 20), translatePoint(155 - 2, 215));
    QPointF upgradePos(translatePoint(50, 193));
    QPointF uniquePos(translatePoint(5 + 2, 144 - 28 - 2));
    QPointF skillPos(translatePoint(5 + 2, 144 + 8));

    // Update lock button geo before parent paint
    if (mLockButton)
    {
        mLockButton->setGeometry(QRect(imgRect.x() + 2, imgRect.y() + 2, 24, 24));
    }

    // Regular label painting (background image)
    QLabel::paintEvent(ev);

    if (mCard.isValid())
    {
        const CGlobalConfig &cfg = CGlobalConfig::getCfg();
        QPainter painter(this);

        // Paint upgrade symbol
        QString cardName = mCard.getName();
        if (mCard.getUpgradeLevel() > 0)
        {
            if (cardName.endsWith(QChar('*')))
            {
                cardName.replace("*", "");
            }
            painter.drawPixmap(upgradePos, QPixmap(cfg.getResourcePicturePath() + "UpgradeIcon.png"));
        }

        // Paint title
        QPen inPen(Qt::white);
        painter.setPen(inPen);
        painter.drawText(titleRect.adjusted(titleRect.height() + 2, -1, 0, -1), Qt::AlignLeft | Qt::AlignVCenter, cardName);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        if (!mTitleIcon.isNull())
        {
            painter.drawPixmap(titleRect.topLeft(), mTitleIcon.scaled(titleRect.height() + 1, titleRect.height() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }

        // Paint unique symbol
        if (mCard.isUnique())
        {
            painter.drawPixmap(uniquePos, QPixmap(":/img/unique.png"));
            //painter.drawText(dx + 2, dySkill - 15, 100, 12, Qt::AlignLeft | Qt::AlignVCenter, "Unique");
        }

        // Paint skill symbols
        const TSkillList &skills = mCard.getSkills();
        int curIdx = 0;
        for (TSkillList::const_iterator i = skills.begin(); i != skills.end(); ++i, ++curIdx)
        {
            const CSkill& curSkill = mCards.getSkillForId(i->getId());
            if (curSkill.isValid())
            {
                painter.drawPixmap(skillPos, QPixmap(cfg.getResourcePicturePath() + curSkill.getPicture() + ".png"));
                skillPos += QPointF(16 + 2, 0);
                //painter.drawText(dx, 80 + i*10, 100, 10, Qt::AlignLeft | Qt::AlignVCenter, curSkill.makeSignature(skills.at(i)));
            }
        }

        // Paint set symbol
        if (mCard.getSet() != EUnknownSet)
        {
            painter.drawPixmap(setRect, QPixmap(cfg.getResourcePicturePath() + mCards.getPictureForCardSet(mCard.getSet())), QRectF(0, 0, 24, 24));
        }


        // Paint delay symbol
        if (mCard.getDelay() > -1)
        {
            painter.drawPixmap(delayRect, QPixmap(cfg.getResourcePicturePath() + "ClockIcon.png"), QRectF(0, 0, 32, 32));
            painter.drawText(delayRect, Qt::AlignCenter, QString("%1").arg(mCard.getDelay()));
        }

        // Paint attack symbol
        if (mCard.hasAttack())
        {
            painter.drawPixmap(attackRect, QPixmap(cfg.getResourcePicturePath() + "AttackIcon.png"), QRectF(0, 0, 20, 20));
            painter.drawText(attackRect.translated(attackRect.width() + 1, 0), Qt::AlignLeft | Qt::AlignVCenter, QString("%1").arg(mCard.getAttack()));
        }

        // Paint health symbol
        if (mCard.hasHealth())
        {
            painter.drawPixmap(healthRect, QPixmap(cfg.getResourcePicturePath() + "HealthIcon.png"), QRectF(0, 0, 20, 20));
            painter.drawText(healthRect.translated(-healthRect.width() - 1, 0), Qt::AlignRight | Qt::AlignVCenter, QString("%1").arg(mCard.getHealth()));
        }

        // Display owned status
        if (cfg.isCardShadingEnabled() && !mCards.isCardOwned(mCard))
        {
            QBrush darkBrush(Qt::black);
            painter.setBrush(darkBrush);
            painter.setOpacity(0.6);
            painter.fillRect(rect(), Qt::SolidPattern);
        }

        SCardStatus status = mCards.getCardStatus(mCard);
        if (cfg.isBlackLabellingEnabled() && status.isBlack)
        {
            // Display black list status
            QBrush darkBrush(Qt::black);
            painter.setOpacity(0.6);
            painter.fillRect(blackRect, darkBrush);
            painter.setOpacity(1.0);
            painter.drawText(blackRect, Qt::AlignCenter, "Blacklisted");
        }
        else if (cfg.isWhiteLabellingEnabled() && status.isWhite)
        {
            // Display white list status
            painter.setPen(QPen(Qt::black));
            painter.setOpacity(0.6);
            painter.fillRect(blackRect, QBrush(Qt::white));
            painter.setOpacity(1.0);
            painter.drawText(blackRect, Qt::AlignCenter, "Whitelisted");
        }
    }
}

void CCardLabel::mousePressEvent(QMouseEvent *ev)
{
    if (mLastLeftClickPos)
    {
        delete mLastLeftClickPos;
        mLastLeftClickPos = 0;
    }
    if (ev->button() == Qt::LeftButton)
    {
        mLastLeftClickPos = new QPointF(ev->globalPos());
    }
}

void CCardLabel::mouseReleaseEvent(QMouseEvent * ev)
{
    if (ev->button() == Qt::RightButton)
    {
        SCardStatus status = mCards.getCardStatus(mCard);
        if (ev->modifiers() == Qt::ControlModifier)
        {
            emit unitCtrlRightClicked(!status.isWhite);
        }
        else
        {
            emit unitRightClicked(!status.isBlack);
        }
    }
    if (mLastLeftClickPos)
    {
        delete mLastLeftClickPos;
        mLastLeftClickPos = 0;
    }
}

void CCardLabel::mouseMoveEvent(QMouseEvent *ev)
{
    if (mLockButton)
    {
        mLockButton->setVisible(mLockButton->isChecked() || mLockButton->geometry().contains(ev->pos()));
    }
    if (mLastLeftClickPos && mCard.isValid())
    {
        QLineF moveLine(ev->globalPos(), *mLastLeftClickPos);
        if (moveLine.length() >= QApplication::startDragDistance())
        {
            QDrag *drag = new QDrag(this);

            QMimeData *dragData = new QMimeData();
            dragData->setText(QString("http://tyrant.40in.net/kg/card.php?id=%1").arg(mCard.getId()));
            drag->setMimeData(dragData);
            drag->setHotSpot(ev->pos());

            const QPixmap *unitImg = pixmap();
            if (unitImg)
            {
                int w = unitImg->width() / 2;
                int h = unitImg->height() / 2;
                QPixmap dragImg(w, h);
                dragImg.fill(Qt::transparent);
                QPainter painter(&dragImg);
                painter.setOpacity(0.75);
                painter.drawPixmap(0, 0, w, h, unitImg->scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                painter.end();
                drag->setPixmap(dragImg);
            }


            if (QApplication::keyboardModifiers() == Qt::ControlModifier || !acceptDrops())
            {
                drag->exec(Qt::CopyAction);
            }
            else
            {
                CCard cardBuf = mCard;
                setCard(CCard::INVALID_CARD);
                if (!drag->exec(Qt::MoveAction))
                {
                    setCard(cardBuf);
                }
            }
            delete mLastLeftClickPos;
            mLastLeftClickPos = 0;
        }
    }
}

void CCardLabel::leaveEvent(QEvent *ev)
{
    if (mLockButton)
    {
        mLockButton->setVisible(mLockButton->isChecked());
    }
    QLabel::leaveEvent(ev);
}

void CCardLabel::dropEvent(QDropEvent *ev)
{
    QString data = ev->mimeData()->text().toLatin1();
    if (data.length() < 50)
    {
        QStringList valueList = data.split("=");
        if (valueList.size() == 2)
        {
            bool ok(true);
            int id = valueList.at(1).toInt(&ok);
            if (ok)
            {
                ev->accept();
                const CCard &card = CCardTable::getCardTable().getCardForId(id);
                if (card.isValid())
                {
                    CCard cardBuf = mCard;
                    setCard(card);
                    CCardLabel *source = dynamic_cast<CCardLabel*>(ev->source());
                    if (source && source != this && ev->dropAction() == Qt::MoveAction)
                    {
                        source->setCard(cardBuf);
                    }
                    emit unitDropped();
                }
            }
        }
    }
}

void CCardLabel::dragMoveEvent(QDragMoveEvent *ev)
{
    QString data = ev->mimeData()->text().toLatin1();
    if (data.length() < 50 && data.startsWith("http://tyrant.40in.net/kg/card.php?id="))
    {
        ev->accept();
    }
}

void CCardLabel::dragEnterEvent(QDragEnterEvent *ev)
{
    QString data = ev->mimeData()->text().toLatin1();
    if (data.length() < 50 && data.startsWith("http://tyrant.40in.net/kg/card.php?id="))
    {
        ev->acceptProposedAction();
    }
}
