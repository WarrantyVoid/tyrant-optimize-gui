#ifndef CFILTERWIDGET_H
#define CFILTERWIDGET_H

#include <QWidget>
#include <QDateTime>
#include "filter/CCardFilterParameters.h"

namespace Ui
{
class CardFilterWidget;
}

class CCardFilterWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CCardFilterWidget(QWidget *parent = 0);
    ~CCardFilterWidget();

public:
    void loadParameterSettings(QSettings &settings);
    void saveParameterSettings(QSettings &settings);
    void scanOwnedCards(QStringList &result) const;
    bool hasOwnedCardsChanged(const QString &fileName) const;

signals:
    void filterUpdated(bool accepted);
    void ownedCardsUpdated(const QStringList &result);

public slots:
    void updateOwnedCardsFile(const QString &fileName);
    void setOwnedCardsFile(const QString &fileName);
    void setDeckBlockage(const CDeck &deck, bool isBlocked);
    void setCardBlackListStatus(const CCard &card, bool isBlack);
    void setCardWhiteListStatus(const CCard &card, bool isWhite);

protected slots:
    void declineFilter();
    void acceptFilter();
    void resetFilter();

 protected:
    void executeFilter();
    void pushParametersToModel();
    
private:
    Ui::CardFilterWidget *mUi;
    CCardFilterParameters mParameters;
    QString mOwnedCardsFile;
    QDateTime mOwnedCardsTime;
};

#endif // CFILTERWIDGET_H
