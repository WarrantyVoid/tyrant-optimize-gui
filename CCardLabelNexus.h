#ifndef CCARDLABELNEXUS_H
#define CCARDLABELNEXUS_H

#include <QObject>
#include "CCardLabel.h"

class CCardLabelNexus : public QObject
{
    Q_OBJECT

public:
    CCardLabelNexus();
    virtual ~CCardLabelNexus();
    static CCardLabelNexus& getCardLabelNexus();

public:
    void registerCardLabel(const CCardLabel* label);
    void unregisterCardLabel(const CCardLabel* label);

signals:
    void blackListStatusToggled(const CCard &card, bool isBlack);
    void whiteListStatusToggled(const CCard &card, bool isWhite);

public slots:
    void setCardLabelBlackListStatus(bool isBlack);
    void setCardLabelWhiteListStatus(bool isWhite);

private:
    static CCardLabelNexus* CARD_LABEL_NEXUS;
};

#endif // CCARDLABELNEXUS_H
