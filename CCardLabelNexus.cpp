#include "CCardLabelNexus.h"

CCardLabelNexus* CCardLabelNexus::CARD_LABEL_NEXUS = 0;

CCardLabelNexus::CCardLabelNexus()
: QObject()
{
}

CCardLabelNexus::~CCardLabelNexus()
{

}

CCardLabelNexus& CCardLabelNexus::getCardLabelNexus()
{
    if (!CARD_LABEL_NEXUS)
    {
        CARD_LABEL_NEXUS = new CCardLabelNexus();
    }
    return *CARD_LABEL_NEXUS;
}

void CCardLabelNexus::registerCardLabel(const CCardLabel* label)
{
    if (label)
    {
        connect(
            label, SIGNAL(unitRightClicked(bool)),
            this, SLOT(setCardLabelBlackListStatus(bool)));
        connect(
            label, SIGNAL(unitCtrlRightClicked(bool)),
            this, SLOT(setCardLabelWhiteListStatus(bool)));
    }
}

void CCardLabelNexus::unregisterCardLabel(const CCardLabel* label)
{
    if (label)
    {
        disconnect(
            label, SIGNAL(unitRightClicked(bool)),
            this, SLOT(setCardLabelBlackListStatus(bool)));
        disconnect(
            label, SIGNAL(unitCtrlRightClicked(bool)),
            this, SLOT(setCardLabelWhiteListStatus(bool)));
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
