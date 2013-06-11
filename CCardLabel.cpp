#include "CCardLabel.h"
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
    setScaledContents(true);   
}

CCardLabel::~CCardLabel()
{
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
    int dx = int(width() / 32.0 + 0.5);        // horizonal border
    int dy = int(height() / 110.0 + 0.5);      // vertical border
    int w = width() - dx * 2;                  // width (without border)
    int h = height() - dy * 2;                 // height (without border)
    int title = int(height() / 9.167 + 0.5);   // title height
    int stat = int(height() / 11.0 + 0.5);     // stat size
    int dySkill = int(height() / 1.447 + 0.5); // skill offset (y)
    int skill = 16;                            // skill size
    int delay = 20;

    if (mLockButton)
    {
        mLockButton->setGeometry(QRect(dx + 2, title + 2, 24, 24));
    }

    QLabel::paintEvent(ev);

    if (mCard.isValid())
    {
        const CGlobalConfig &cfg = CGlobalConfig::getCfg();
        QPainter painter(this);

        QPen inPen(Qt::white);
        painter.setPen(inPen);

        QString cardName = mCard.getName();
        if (mCard.getUpgradeLevel() > 0)
        {
            if (cardName.endsWith(QChar('*')))
            {
                cardName.replace("*", "");
            }
            painter.drawPixmap((w - 20) / 2 , h - stat / 2 - 7, QPixmap(cfg.getResourcePicturePath() + "UpgradeIcon.png"));
        }
        painter.drawText(dx + title + 2, dy - 1, w - title - 2, title - 1, Qt::AlignLeft | Qt::AlignVCenter, cardName);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        if (!mTitleIcon.isNull())
        {
            painter.drawPixmap(dx, dy, mTitleIcon.scaled(title, title, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }

        if (mCard.isUnique())
        {
            painter.drawPixmap(dx, dySkill - 20, QPixmap(":/img/unique.png"));
            //painter.drawText(dx + 2, dySkill - 15, 100, 12, Qt::AlignLeft | Qt::AlignVCenter, "Unique");
        }

        const TCardSkills &skills = mCard.getSkills();
        int curIdx = 0;
        for (TCardSkills::const_iterator i = skills.begin(); i != skills.end(); ++i, ++curIdx)
        {
            const CSkill& curSkill = mCards.getSkillForId(i->getId());
            if (curSkill.isValid())
            {
                QPoint skillPos(dx + 2 + curIdx * (2 + skill), dySkill);
                painter.drawPixmap(skillPos, QPixmap(cfg.getResourcePicturePath() + curSkill.getPicture() + ".png"));
                //painter.drawText(dx, 80 + i*10, 100, 10, Qt::AlignLeft | Qt::AlignVCenter, curSkill.makeSignature(skills.at(i)));
            }
        }

        if (mCard.getSet() != EUnknownSet)
        {
            QRect cardSetBounds(dx + w - skill, dySkill, skill, skill);
            painter.drawPixmap(cardSetBounds, QPixmap(cfg.getResourcePicturePath() + mCards.getPictureForCardSet(mCard.getSet())));
        }


        if (mCard.getDelay() > -1)
        {
            QRect delayBounds(w + dx - delay + 1, title - 1, delay, delay);
            painter.drawPixmap(delayBounds, QPixmap(cfg.getResourcePicturePath() + "ClockIcon.png").scaled(delay, delay, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            painter.drawText(delayBounds, Qt::AlignCenter, QString("%1").arg(mCard.getDelay()));
        }

        if (mCard.hasAttack())
        {
            painter.drawPixmap(dx, h - stat, QPixmap(cfg.getResourcePicturePath() + "AttackIcon.png").scaled(stat, stat, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            painter.drawText(dx + stat + 2, h - stat - 1, 2 * stat, stat - 1, Qt::AlignLeft | Qt::AlignVCenter, QString("%1").arg(mCard.getAttack()));
        }

        if (mCard.hasHealth())
        {
            painter.drawPixmap(w + dx - stat, h - stat, QPixmap(cfg.getResourcePicturePath() + "HealthIcon.png").scaled(stat, stat, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            painter.drawText(w + dx - 3 * stat - 2, h - stat - 1, 2 * stat, stat - 1, Qt::AlignRight | Qt::AlignVCenter, QString("%1").arg(mCard.getHealth()));
        }

        if (cfg.isCardShadingEnabled() && !mCards.isCardOwned(&mCard))
        {
            QBrush darkBrush(Qt::black);
            painter.setBrush(darkBrush);
            painter.setOpacity(0.6);
            painter.fillRect(rect(), Qt::SolidPattern);
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
