#include "CCardLabel.h"
#include "CCardLabelNexus.h"
#include "CGlobalConfig.h"
#include "model/CCardTable.h"
#include <QApplication>
#include <QPaintEvent>
#include <QPainter>
#include <QMenu>
#include <QDrag>
#include <QMimeData>
#include <QToolTip>

CCardLabel::CCardLabel(QWidget *parent, bool isVirtualCard)
: QLabel(parent)
, mCards(CCardTable::getCardTable())
, mTitleIcon()
, mLastLeftClickPos(0)
, mLockButton(0)
, mIsVirtual(isVirtualCard)
{
    CCardLabelNexus::getCardLabelNexus().registerCardLabel(this);
    setScaledContents(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(displayContextMenu(QPoint)));
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
}

const CCard& CCardLabel::getCard() const
{
    return mCard;
}

QMimeData *CCardLabel::createCardLabelDropData(const CCardLabel &label)
{
    QMimeData *data = new QMimeData();
    data->setHtml(QString("@CCardLabel:id=%1;locked=%2")
                  .arg(label.getCard().getId())
                  .arg(label.isLocked() ? 1 : 0));
    return data;
}

bool CCardLabel::isCardLabelDropData(const QMimeData *data)
{
    if (data && data->hasHtml())
    {
        QString textData = data->html().toLatin1();
        if (textData.length() < 50 && textData.startsWith("@CCardLabel:id="))
        {
            return true;
        }
    }
    return false;
}

void CCardLabel::displayContextMenu(const QPoint &/*pos*/)
{
    SCardStatus status = mCards.getCardStatus(mCard);
    QAction *blackListAction = new QAction("Black list", this);
    blackListAction->setCheckable(true);
    blackListAction->setChecked(status.isBlack);
    blackListAction->setEnabled(mCard.isValid());
    QAction *whiteListAction = new QAction("White list", this);
    whiteListAction->setCheckable(true);
    whiteListAction->setChecked(status.isWhite);
    whiteListAction->setEnabled(mCard.isValid());
    connect(blackListAction, SIGNAL(triggered(bool)), this, SLOT(actionToggleBlack(bool)));
    connect(whiteListAction, SIGNAL(triggered(bool)), this, SLOT(actionToggleWhite(bool)));

    QMenu menu ;
    menu.addAction(blackListAction);
    menu.addAction(whiteListAction);
    menu.exec(QCursor::pos());
}

void CCardLabel::actionToggleBlack(bool isBlack)
{
    emit unitRightClicked(isBlack);
}

void CCardLabel::actionToggleWhite(bool isWhite)
{
    emit unitCtrlRightClicked(isWhite);
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
    QRectF ownedRect(translatePoint(155 - 60 - 2, 144 - 28), translatePoint(155 - 2, 144));
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
                QPixmap skillPixmap;
                if (!skillPixmap.load(cfg.getResourcePicturePath() + curSkill.getPicture() + ".png"))
                {
                    skillPixmap.load(cfg.getCustomPicturePath() + curSkill.getPicture() + ".png");
                }
                painter.drawPixmap(skillPos, skillPixmap);
                skillPos += QPointF(16 + 2, 0);
                //painter.drawText(dx, 80 + i*10, 100, 10, Qt::AlignLeft | Qt::AlignVCenter, curSkill.makeSignature(skills.at(i)));
            }
        }

        // Paint set symbol
        if (mCard.getSet() != EUnknownSet)
        {
            QPixmap setPixmap;
            if (!setPixmap.load(cfg.getResourcePicturePath() + mCards.getPictureForCardSet(mCard.getSet())))
            {
                setPixmap.load(cfg.getCustomPicturePath() + mCards.getPictureForCardSet(mCard.getSet()));
            }
            painter.drawPixmap(setRect, setPixmap, QRectF(0, 0, 24, 24));
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

            painter.drawText(healthRect.adjusted(-2 * healthRect.width() - 1, 0, -healthRect.width() - 1, 0), Qt::AlignRight | Qt::AlignVCenter, QString("%1").arg(mCard.getHealth()));
        }

        if (!mIsVirtual)
        {
            // Display owned status
            SCardStatus status = mCards.getCardStatus(mCard);
            {
                painter.setPen(Qt::green);
                QString ownageStr("x0");
                if (status.numOwned > 0)
                {
                    ownageStr = QString("x%1").arg(status.numOwned);
                }
                painter.drawText(ownedRect, Qt::AlignRight | Qt::AlignVCenter, ownageStr);
            }
            if (cfg.isCardShadingEnabled() && status.numOwned <= 0)
            {
                painter.setPen(Qt::NoPen);
                painter.setBrush(Qt::black);
                painter.setOpacity(0.6);
                painter.drawRoundedRect(rect(), 2.0, 2.0);
            }
            if (cfg.isBlackLabellingEnabled() && status.isBlack)
            {
                // Display black list status
                painter.setPen(Qt::white);
                painter.setOpacity(0.6);
                painter.fillRect(blackRect, QBrush(Qt::black));
                painter.setOpacity(1.0);
                painter.drawText(blackRect, Qt::AlignCenter, "Blacklisted");
            }
            else if (cfg.isWhiteLabellingEnabled() && status.isWhite)
            {
                // Display white list status
                painter.setPen(Qt::black);
                painter.setOpacity(0.6);
                painter.fillRect(blackRect, QBrush(Qt::white));
                painter.setOpacity(1.0);
                painter.drawText(blackRect, Qt::AlignCenter, "Whitelisted");
            }
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

void CCardLabel::mouseReleaseEvent(QMouseEvent * /*ev*/)
{
    if (mLastLeftClickPos)
    {
        delete mLastLeftClickPos;
        mLastLeftClickPos = 0;
    }
}

void CCardLabel::mouseDoubleClickEvent(QMouseEvent * ev)
{
    if (ev && ev->button() == Qt::LeftButton)
    {
        if (!mLockButton
            || !mLockButton->isVisible()
            || !mLockButton->geometry().contains(ev->pos()))
        {
            emit unitDoubleClicked();
        }
    }
}

void CCardLabel::mouseMoveEvent(QMouseEvent *ev)
{
    if (mLockButton)
    {
        mLockButton->setVisible(mLockButton->isChecked()
            || (mCard.isValid() && mLockButton->geometry().contains(ev->pos())));
    }
    if (mLastLeftClickPos && mCard.isValid())
    {
        QLineF moveLine(ev->globalPos(), *mLastLeftClickPos);
        if (moveLine.length() >= QApplication::startDragDistance())
        {
            QDrag *drag = new QDrag(this);
            QMimeData *dragData = createCardLabelDropData(*this);
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
                bool lockBuf = isLocked();
                setCard(CCard::INVALID_CARD);
                setLocked(false);
                if (!drag->exec(Qt::MoveAction))
                {
                    setCard(cardBuf);
                    setLocked(lockBuf);
                }
            }
            delete mLastLeftClickPos;
            mLastLeftClickPos = 0;
            emit unitDragged();
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
    if (ev && isCardLabelDropData(ev->mimeData()))
    {
        QString textData = ev->mimeData()->html().toLatin1();
        QStringList valueList = textData.split(QRegExp("=|;"));
        if (valueList.size() == 4)
        {
            bool ok1(true);
            bool ok2(true);
            int id = valueList.at(1).toInt(&ok1);
            bool locked = valueList.at(3).toInt(&ok2) == 1;
            if (ok1 && ok2)
            {
                ev->accept();
                const CCard &card = CCardTable::getCardTable().getCardForId(id);
                if (card.isValid())
                {
                    CCard cardBuf = mCard;
                    bool lockBuf = isLocked();
                    setCard(card);
                    setLocked(locked);
                    CCardLabel *source = dynamic_cast<CCardLabel*>(ev->source());
                    if (source && source != this && ev->dropAction() == Qt::MoveAction)
                    {
                        source->setCard(cardBuf);
                        source->setLocked(lockBuf);
                    }
                    emit unitDropped();
                }
            }
        }
    }
}

void CCardLabel::dragMoveEvent(QDragMoveEvent *ev)
{
    if (ev && isCardLabelDropData(ev->mimeData()))
    {
        ev->accept();
    }
}

void CCardLabel::dragEnterEvent(QDragEnterEvent *ev)
{
    if (ev && isCardLabelDropData(ev->mimeData()))
    {
        ev->acceptProposedAction();
    }
}
