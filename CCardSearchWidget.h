#ifndef CCARDSEARCHWIDGET_H
#define CCARDSEARCHWIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include <QList>
#include "model/CCardTable.h"
#include "CCardSearchParameters.h"

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
    void cardSelected(unsigned int cardId);

public slots:
    void updateView();
    void updateLayout();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual bool eventFilter(QObject *obj, QEvent *e);
    
private:
    static const int NUM_RESULT_WIDGETS;
    Ui::CardSearchWidget *mUi;
    QGraphicsScene* mScene;
    CCardSearchParameters mSearchParameters;
    QList<QGraphicsProxyWidget*> mResultWidgets;    
    bool mSupressUpdates;
};

#endif // CCARDSEARCHWIDGET_H
