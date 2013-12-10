#include "CCardLabelNexus.h"
#include "CCardLabel.h"
#include "model/CCardTable.h"
#include "CGlobalConfig.h"
#include <QApplication>
#include <QHelpEvent>
#include <QDesktopWidget>
#include <QBoxLayout>
#include <QDialog>


CCardLabelNexus* CCardLabelNexus::CARD_LABEL_NEXUS = 0;

CCardLabelNexus::CCardLabelNexus()
: QObject()
, mCardLabelToolTip(new QDialog())
, mCardLabelToolTipText(new QLabel())
, mCardLabelToolTipLayout(0)
, mCardLabelToolTipSummonLabel(0)
{
    mCardLabelToolTip->setWindowFlags(Qt::ToolTip);
    mCardLabelToolTip->setStyleSheet("QDialog{background-color: rgb(255, 255, 219);border-color: rgb(0, 0, 0);border-width: 1px;border-style: solid}");
    mCardLabelToolTipLayout = new QBoxLayout(QBoxLayout::TopToBottom, mCardLabelToolTip);
    mCardLabelToolTipLayout->setSpacing(5);
    mCardLabelToolTipLayout->setMargin(5);
    mCardLabelToolTipText->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    mCardLabelToolTipLayout->addWidget(mCardLabelToolTipText);
}

CCardLabelNexus::~CCardLabelNexus()
{
    delete mCardLabelToolTip;
    mCardLabelToolTip = 0;
}

CCardLabelNexus& CCardLabelNexus::getCardLabelNexus()
{
    if (!CARD_LABEL_NEXUS)
    {
        CARD_LABEL_NEXUS = new CCardLabelNexus();
    }
    return *CARD_LABEL_NEXUS;
}

void CCardLabelNexus::registerCardLabel(CCardLabel* label)
{
    if (label)
    {
        connect(
            label, SIGNAL(unitRightClicked(bool)),
            this, SLOT(setCardLabelBlackListStatus(bool)));
        connect(
            label, SIGNAL(unitCtrlRightClicked(bool)),
            this, SLOT(setCardLabelWhiteListStatus(bool)));
        label->installEventFilter(this);
    }
}

void CCardLabelNexus::unregisterCardLabel(CCardLabel *label)
{
    if (label)
    {
        disconnect(
            label, SIGNAL(unitRightClicked(bool)),
            this, SLOT(setCardLabelBlackListStatus(bool)));
        disconnect(
            label, SIGNAL(unitCtrlRightClicked(bool)),
            this, SLOT(setCardLabelWhiteListStatus(bool)));
        label->removeEventFilter(this);
    }
}

void CCardLabelNexus::setCardLabelBlackListStatus(bool isBlack)
{
    CCardLabel* sendLabel = dynamic_cast<CCardLabel*>(QObject::sender());
    if (sendLabel)
    {
        emit blackListStatusToggled(sendLabel->getCard(), isBlack);
    }
}

void CCardLabelNexus::setCardLabelWhiteListStatus(bool isWhite)
{
    CCardLabel* sendLabel = dynamic_cast<CCardLabel*>(QObject::sender());
    if (sendLabel)
    {
        emit whiteListStatusToggled(sendLabel->getCard(), isWhite);
    }
}

void CCardLabelNexus::showCardLabelToolTip(const CCard &card, int x, int y)
{
    if (!mCardLabelToolTip->isVisible() && card.isValid())
    {
        const CGlobalConfig &cfg = CGlobalConfig::getCfg();
        const CCardTable &cardTable = CCardTable::getCardTable();

        mCardLabelToolTipSummonLabel->setVisible(false);
        const QList<CCardSkill> &skills = card.getSkills();
        QStringList skillDescr;
        skillDescr.append(QString("<b>%1</b><br>").arg(card.getName()));
        QString subTitleStr = card.isUnique() ? "Unique " : "";
        switch(card.getFaction())
        {
        case EImperialFaction: subTitleStr += "Imperial"; break;
        case EBloodthirstyFaction: subTitleStr += "Bloodthirsty"; break;
        case EXenoFaction: subTitleStr += "Xeno"; break;
        case ERighteousFaction: subTitleStr += "Righteous"; break;
        case ERaiderFaction: subTitleStr += "Raider"; break;
        default: break;
        }
        skillDescr.append(QString("<i>%1</i><table valign='middle'>").arg(subTitleStr));
        for (int i = 0; i < skills.size(); ++i)
        {
            const CSkill& curSkill = cardTable.getSkillForId(skills.at(i).getId());
            if (curSkill.isValid())
            {
                skillDescr.append(QString("<tr><td><img src='%1.png'/></td><td> %2</td></tr>")
                    .arg(cfg.getResourcePicturePath() + curSkill.getPicture())
                    .arg(curSkill.makeSignature(skills.at(i), true)));
                if (curSkill.isSummon())
                {
                    const CCard &summonCard = cardTable.getCardForId(skills.at(i).getX());
                    mCardLabelToolTipSummonLabel->setCard(summonCard);
                    mCardLabelToolTipSummonLabel->setVisible(true);
                }
            }
        }
        skillDescr.append(QString("</table>"));
        mCardLabelToolTipText->setText(skillDescr.join(""));
        mCardLabelToolTipLayout->activate();
        mCardLabelToolTip->adjustSize();

        int reqW = mCardLabelToolTip->width() + 5;
        int reqH = mCardLabelToolTip->height() + 20;
        int xRel = (QApplication::desktop()->width() - x < reqW)
            ? -reqW
            : 5;
        int yRel = (QApplication::desktop()->height() - y < reqH)
            ? -reqH
            : 20;
        mCardLabelToolTip->setGeometry(x + xRel, y + yRel, 80, 40);
        mCardLabelToolTip->setVisible(true);
    }
}

void CCardLabelNexus::hideCardLabelToolTip()
{
    mCardLabelToolTip->setVisible(false);
}

bool CCardLabelNexus::eventFilter(QObject *obj, QEvent *e)
{
    switch (e->type())
    {
        case QEvent::ToolTip:
        {
            QHelpEvent *he = static_cast<QHelpEvent *>(e);
            CCardLabel *cardLabel = dynamic_cast<CCardLabel*>(obj);
            if (e && cardLabel)
            {
                addCardLabelToolTipSummonLabels();
                showCardLabelToolTip(cardLabel->getCard(), he->globalX(),  he->globalY());
                he->accept();
            }
            return true;
        }
        //case QEvent::FocusOut:
        case QEvent::Leave:
            hideCardLabelToolTip();
            return QObject::eventFilter(obj, e);
        default:
        {
            // standard event processing
            return QObject::eventFilter(obj, e);
        }
    }
}

void CCardLabelNexus::addCardLabelToolTipSummonLabels()
{
    if (mCardLabelToolTipSummonLabel == 0 && mCardLabelToolTipSummonLabel == 0)
    {
        QBoxLayout* hLayout = new QBoxLayout(QBoxLayout::LeftToRight);
        mCardLabelToolTipSummonLabel = new CCardLabel(0, true);
        mCardLabelToolTipSummonLabel->setMaximumSize(80, 110);
        hLayout->addWidget(mCardLabelToolTipSummonLabel);
        hLayout->setMargin(0);
        hLayout->addStretch();
        mCardLabelToolTipLayout->addLayout(hLayout);
    }
}
