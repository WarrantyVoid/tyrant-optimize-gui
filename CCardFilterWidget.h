#ifndef CFILTERWIDGET_H
#define CFILTERWIDGET_H

#include <QWidget>
#include <QDateTime>
#include "CCardFilterParameters.h"

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

protected slots:
    void declineFilter();
    void acceptFilter();

 protected:
    void executeFilter();
    
private:
    Ui::CardFilterWidget *mUi;
    CCardFilterParameters mParameters;
    QString mOwnedCardsFile;
    QDateTime mOwnedCardsTime;
};

#endif // CFILTERWIDGET_H
