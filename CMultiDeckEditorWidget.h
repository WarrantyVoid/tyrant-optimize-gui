#ifndef CMULTIDECKEDITORWIDGET_H
#define CMULTIDECKEDITORWIDGET_H

#include <QWidget>
#include "model/CCardTable.h"

namespace Ui
{
class MultiDeckEditorWidget;
}

class CMultiDeckEditorWidget : public QWidget
{
    Q_OBJECT
    
public:
    CMultiDeckEditorWidget(int num,QWidget *parent = 0);
    ~CMultiDeckEditorWidget();

public:
    void setToolTipHandler(QObject *handler);
    QString getResult();
    void updateHistory();
    void reset();    

signals:
    void totalNumIncreased(int totalNum);
    void relativeFractionChanged(double oldFraction, double fractionDif);

public slots:
    void updateDeck(const QString &deck);
    void updateBaseFraction(int totalNum);
    void updateRelativeFraction(double newFraction);
    void updateRelativeFraction(double oldFraction, double fractionDif);

protected slots:
    void on_deckFractionSlider_sliderMoved(int value);

private:
    bool isDoubleEqual(double d1, double d2);
    
private:
    Ui::MultiDeckEditorWidget *mUi;
    CCardTable &mCards;
    int mNum;
    int mTotalNum;
    double mBaseFraction;
    double mFractionOffset;
};

#endif // CMULTIDECKEDITORWIDGET_H
