#ifndef CCARDSEARCHWIDGET_H
#define CCARDSEARCHWIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include <QComboBox>
#include <QList>
#include "filter/CCardSearchParameters.h"
#include "model/CCardTable.h"

namespace Ui {
class CardSearchWidget;
}

class CCardSearchWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CCardSearchWidget(QWidget *parent = 0);
    ~CCardSearchWidget();

public:
    void loadParameterSettings(QSettings &settings);
    void saveParameterSettings(QSettings &settings);

signals:
    void cardSelected(TCardId cardId);

public slots:
    void switchAttackOption();
    void switchHpOption();
    void switchUpgradeOption();
    void updateView(ECardStatusUpdate status = EOwnedStatusUpdate); 
    void updateLayout();

protected slots:
    void onCardDoubleClicked();
    void onCardDragged();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    void updateBoxHistory(QComboBox* box);
    
private:
    static const int NUM_RESULT_WIDGETS;
    Ui::CardSearchWidget *mUi;
    QGraphicsScene* mScene;
    CCardSearchParameters mSearchParameters;
    QList<QGraphicsProxyWidget*> mResultWidgets;    
    bool mSupressUpdates;
};

#endif // CCARDSEARCHWIDGET_H
