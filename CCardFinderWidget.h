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

public slots:
    void updateView(const QString &search);
    void updateLayout();

protected:
    void resizeEvent(QResizeEvent *event);
    
private:
    static const int NUM_RESULT_WIDGETS;
    Ui::CardFinderWidget *mUi;
    QGraphicsScene* mScene;
    QList<QGraphicsProxyWidget*> mResultWidgets;
};

#endif // CCARDFINDERWIDGET_H
