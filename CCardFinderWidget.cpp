#include "CCardFinderWidget.h"
#include "ui_CardFinderWidget.h"
#include "CCardTable.h"
#include "CUnitLabel.h"
#include <QGraphicsProxyWidget>

const int CCardFinderWidget::NUM_RESULT_WIDGETS = 20;

CCardFinderWidget::CCardFinderWidget(QWidget *parent)
: QWidget(parent)
, mUi(new Ui::CardFinderWidget)
, mScene(new QGraphicsScene())
, mResultWidgets()
{
    mUi->setupUi(this);
    mUi->finderView->setScene(mScene);

    for (int i = 0; i < NUM_RESULT_WIDGETS; ++i)
    {
        CUnitLabel* newLabel = new CUnitLabel();
        newLabel->setFixedSize(80, 110);
        mResultWidgets.push_back(mScene->addWidget(newLabel));
    }
    updateLayout();
    int widthView = mUi->finderView->viewport()->width();
    int widthWidget = mUi->finderView->width();
    mUi->finderView->setMinimumWidth(widthView + widthWidget - widthView);

    connect(
        mUi->finderBox, SIGNAL(activated(const QString&)),
        this, SLOT(updateView(const QString&)));
}

CCardFinderWidget::~CCardFinderWidget()
{
    delete mUi;
}

void CCardFinderWidget::updateView(const QString &search)
{
    CCardTable &cards = CCardTable::getCardTable();
    QList<CCard> foundCards;
    cards.searchCards(search, foundCards, NUM_RESULT_WIDGETS);
    for (int i = 0; i < NUM_RESULT_WIDGETS; ++i)
    {
        if (i < foundCards.size())
        {
            static_cast<CUnitLabel*>(mResultWidgets[i]->widget())->setCard(foundCards[i]);
            mResultWidgets[i]->widget()->setVisible(true);
        }
        else
        {
            mResultWidgets[i]->widget()->setVisible(false);
        }
    }
}

void CCardFinderWidget::updateLayout()
{
    int viewWidth = mUi->finderView->viewport()->size().width();
    int numItemsX = qMax(viewWidth / 80, 1);
    int curItemX = 0;
    int curItemY = 0;

    mScene->setSceneRect(0, 0, numItemsX * 80, NUM_RESULT_WIDGETS * 110 / numItemsX);
    for (int i = 0; i < NUM_RESULT_WIDGETS; ++i)
    {
        mResultWidgets[i]->setPos(curItemX * 80, curItemY * 110);
        if (curItemX < numItemsX - 1)
        {
            ++curItemX;
        }
        else
        {
            curItemX = 0;
            ++curItemY;
        }
    }
}

void CCardFinderWidget::resizeEvent(QResizeEvent *event)
{
    //const QSize &sizeOld = event->oldSize();
    //const QSize &sizeNew = event->size();
    //if (sizeNew.width() / 80 != sizeOld.width() / 80)
    {
        updateLayout();
    }
}
