#ifndef CCARDFINDERWIDGET_H
#define CCARDFINDERWIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include <QList>
#include "CCardTable.h"

namespace Ui {
class CardFinderWidget;
}

class CCardFinderWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CCardFinderWidget(QWidget *parent = 0);
    ~CCardFinderWidget();

signals:
    void cardSelected(unsigned int cardId);

public slots:
    void updateView(const QString &search);
    void updateLayout();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual bool eventFilter(QObject *obj, QEvent *e);
    
private:
    static const int NUM_RESULT_WIDGETS;
    Ui::CardFinderWidget *mUi;
    QGraphicsScene* mScene;
    QList<QGraphicsProxyWidget*> mResultWidgets;
};

#endif // CCARDFINDERWIDGET_H
