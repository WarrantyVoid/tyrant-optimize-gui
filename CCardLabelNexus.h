#ifndef CCARDLABELNEXUS_H
#define CCARDLABELNEXUS_H

#include <QObject>

class QDialog;
class QLabel;
class QBoxLayout;
class CCardLabel;
class CCard;

class CCardLabelNexus : public QObject
{
    Q_OBJECT

public:
    CCardLabelNexus();
    virtual ~CCardLabelNexus();
    static CCardLabelNexus& getCardLabelNexus();

public:
    void updateCardPictures(const CCard & card);
    void registerCardLabel(CCardLabel *label);
    void unregisterCardLabel( CCardLabel* label);

signals:
    void blackListStatusToggled(const CCard &card, bool isBlack);
    void whiteListStatusToggled(const CCard &card, bool isWhite);
    void pictureDownloaded(const CCard &card);

public slots:
    void setCardLabelBlackListStatus(bool isBlack);
    void setCardLabelWhiteListStatus(bool isWhite);
    void showCardLabelToolTip(const CCard &card, int x, int y);
    void hideCardLabelToolTip();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *e);

private:
    void addCardLabelToolTipSummonLabels();

private:
    static CCardLabelNexus* CARD_LABEL_NEXUS;

private:
    QDialog *mCardLabelToolTip;
    QLabel *mCardLabelToolTipText;
    QBoxLayout *mCardLabelToolTipLayout;
    CCardLabel *mCardLabelToolTipSummonLabel;
};

#endif // CCARDLABELNEXUS_H
