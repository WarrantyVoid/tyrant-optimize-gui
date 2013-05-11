#include "CCardSearchWidget.h"
#include "ui_CardSearchWidget.h"
#include "model/CCardTable.h"
#include "CCardLabel.h"
#include <QGraphicsProxyWidget>

const int CCardSearchWidget::NUM_RESULT_WIDGETS = 50;

CCardSearchWidget::CCardSearchWidget(QWidget *parent)
: QWidget(parent)
, mUi(new Ui::CardSearchWidget)
, mScene(new QGraphicsScene())
, mSearchParameters()
, mResultWidgets()
, mSupressUpdates(false)
{
    mUi->setupUi(this);
    mUi->finderView->setScene(mScene);
    for (int i = 0; i < NUM_RESULT_WIDGETS; ++i)
    {
        CCardLabel* newLabel = new CCardLabel();
        newLabel->setFixedSize(80, 110);
        newLabel->installEventFilter(this);
        mResultWidgets.push_back(mScene->addWidget(newLabel));
    }
    updateLayout();
    int widthView = mUi->finderView->viewport()->width();
    int widthWidget = mUi->finderView->width();
    mUi->finderView->setMinimumWidth(widthView + widthWidget - widthView);

    connect(
        mUi->nameBox, SIGNAL(editTextChanged(const QString&)),
        this, SLOT(updateView()));
    connect(
        mUi->skillBox, SIGNAL(editTextChanged(const QString&)),
        this, SLOT(updateView()));

    connect(
        mUi->legendaryButton, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->rareButton, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->uncommonButton, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->commonButton, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));

    connect(
        mUi->assaultButton, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->structureButton, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->commanderButton, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->actionButton, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));

    connect(
        mUi->attackSlider, SIGNAL(valueChanged(int)),
        this, SLOT(updateView()));
    connect(
        mUi->hpSlider, SIGNAL(valueChanged(int)),
        this, SLOT(updateView()));

    connect(
        mUi->bloodthirstyButton, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->imperialButton, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->raiderButton, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->righteousButton, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->xenoButton, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));

    connect(
        mUi->timer0Button, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->timer1Button, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->timer2Button, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->timer3Button, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
    connect(
        mUi->timer4Button, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));
}

CCardSearchWidget::~CCardSearchWidget()
{
    delete mUi;
}

void CCardSearchWidget::loadParameterSettings(QSettings &settings)
{
    mSupressUpdates = true;
    mSearchParameters.fetchFromSettings(settings);
    mSearchParameters.updateUi(*mUi);
    mSupressUpdates = false;
    updateView();
}

void CCardSearchWidget::saveParameterSettings(QSettings &settings)
{
    mSearchParameters.fetchFromUi(*mUi);
    mSearchParameters.updateSettings(settings);
}

void CCardSearchWidget::updateView()
{
    if (!mSupressUpdates)
    {
        CCardTable &cards = CCardTable::getCardTable();
        QList<CCard*> foundCards;
        mSearchParameters.fetchFromUi(*mUi);
        cards.searchCards(mSearchParameters, foundCards, NUM_RESULT_WIDGETS);
        for (int i = 0; i < NUM_RESULT_WIDGETS; ++i)
        {
            if (i < foundCards.size())
            {
                static_cast<CCardLabel*>(mResultWidgets[i]->widget())->setCard(*foundCards[i]);
                mResultWidgets[i]->widget()->setVisible(true);
            }
            else
            {
                mResultWidgets[i]->widget()->setVisible(false);
            }
        }
    }
}

void CCardSearchWidget::updateLayout()
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

void CCardSearchWidget::showEvent(QShowEvent */*event*/)
{
    updateLayout();
}

void CCardSearchWidget::resizeEvent(QResizeEvent */*event*/)
{
    updateLayout();
}

bool CCardSearchWidget::eventFilter(QObject *obj, QEvent *e)
{
    switch (e->type())
    {
        case QEvent::MouseButtonDblClick:
        {
            CCardLabel *cardLabel = static_cast<CCardLabel*>(obj);
            const CCard &card = cardLabel->getCard();
            if (card.isValid())
            {
                emit cardSelected(card.getId());
            }
            return true;
        }
        default:
        {
            // standard event processing
            return QObject::eventFilter(obj, e);
        }
    }
}
