#include "CCardSearchWidget.h"
#include "ui_CardSearchWidget.h"
#include "model/CCardTable.h"
#include "CCardLabel.h"
#include <QGraphicsProxyWidget>
#include <QLineEdit>

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
        connect(newLabel, SIGNAL(unitDragged()),
            this, SLOT(onCardDragged()));
        connect(newLabel, SIGNAL(unitDoubleClicked()),
            this, SLOT(onCardDoubleClicked()));
        mResultWidgets.push_back(mScene->addWidget(newLabel));
    }
    updateLayout();
    int widthView = mUi->finderView->viewport()->width();
    int widthWidget = mUi->finderView->width();
    mUi->finderView->setMinimumWidth(widthView + widthWidget - widthView);

    connect(
        mUi->hpPlusButton, SIGNAL(clicked()),
        this, SLOT(switchHpOption()));
    connect(
        mUi->hpEqualButton, SIGNAL(clicked()),
        this, SLOT(switchHpOption()));
    connect(
        mUi->hpMinusButton, SIGNAL(clicked()),
        this, SLOT(switchHpOption()));
    connect(
        mUi->attackPlusButton, SIGNAL(clicked()),
        this, SLOT(switchAttackOption()));
    connect(
        mUi->attackEqualButton, SIGNAL(clicked()),
        this, SLOT(switchAttackOption()));
    connect(
        mUi->attackMinusButton, SIGNAL(clicked()),
        this, SLOT(switchAttackOption()));


    connect(
        mUi->skillBox, SIGNAL(editTextChanged(const QString&)),
        this, SLOT(updateView()));

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

    connect(
        mUi->uniqueButton, SIGNAL(clicked(bool)),
        this, SLOT(updateView()));

    connect(
        mUi->upgradeAllButton, SIGNAL(clicked()),
        this, SLOT(switchUpgradeOption()));
    connect(
        mUi->upgradeNoneButton, SIGNAL(clicked()),
        this, SLOT(switchUpgradeOption()));
    connect(
        mUi->upgrade0Button, SIGNAL(clicked()),
        this, SLOT(switchUpgradeOption()));
    connect(
        mUi->upgrade1Button, SIGNAL(clicked()),
        this, SLOT(switchUpgradeOption()));
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

void CCardSearchWidget::switchAttackOption()
{
    int numStates = mUi->attackStackedWidget->count();
    int curState = mUi->attackStackedWidget->currentIndex();
    if (curState < numStates - 1)
    {
       mUi->attackStackedWidget->setCurrentIndex(curState + 1);
    }
    else
    {
        mUi->attackStackedWidget->setCurrentIndex(0);
    }
    updateView();
}

void CCardSearchWidget::switchHpOption()
{
    int numStates = mUi->hpStackedWidget->count();
    int curState = mUi->hpStackedWidget->currentIndex();
    if (curState < numStates - 1)
    {
       mUi->hpStackedWidget->setCurrentIndex(curState + 1);
    }
    else
    {
        mUi->hpStackedWidget->setCurrentIndex(0);
    }
    updateView();
}

void CCardSearchWidget::switchUpgradeOption()
{
    int numStates = mUi->upgradeStackedWidget->count();
    int curState = mUi->upgradeStackedWidget->currentIndex();
    if (curState < numStates - 1)
    {
       mUi->upgradeStackedWidget->setCurrentIndex(curState + 1);
    }
    else
    {
        mUi->upgradeStackedWidget->setCurrentIndex(0);
    }
    updateView();
}

void CCardSearchWidget::updateView(ECardStatusUpdate status)
{
    if (!mSupressUpdates)
    {
        if (status == EOwnedStatusUpdate)
        {
            CCardTable &cards = CCardTable::getCardTable();
            QList<CCard*> foundCards;
            QList<CCard*> ownedCards;
            mSearchParameters.fetchFromUi(*mUi);
            cards.searchCards(mSearchParameters, foundCards);
            int numCardsFound = foundCards.size();

            for (int i = foundCards.size() - 1; i > -1; --i)
            {
                if (cards.isCardOwned(*foundCards[i]))
                {
                    ownedCards.push_back(foundCards[i]);
                    foundCards.removeAt(i);
                }
            }

            for (int i = 0; i < NUM_RESULT_WIDGETS; ++i)
            {
                if (i < ownedCards.size())
                {
                    static_cast<CCardLabel*>(mResultWidgets[i]->widget())->setCard(*ownedCards[i]);
                    mResultWidgets[i]->widget()->setVisible(true);
                    if (i == 0)
                    {
                        mUi->finderView->ensureVisible(mResultWidgets[i]);
                    }
                }
                else if (i - ownedCards.size() < foundCards.size())
                {
                    static_cast<CCardLabel*>(mResultWidgets[i]->widget())->setCard(*foundCards[i - ownedCards.size()]);
                    mResultWidgets[i]->widget()->setVisible(true);
                    if (i == 0)
                    {
                        mUi->finderView->ensureVisible(mResultWidgets[i]);
                    }
                }
                else
                {
                    mResultWidgets[i]->widget()->setVisible(false);
                }
            }
            QString finderResult = QString("Search result: Displaying %1 of %2 matches")
                .arg(qMin(NUM_RESULT_WIDGETS, numCardsFound))
                .arg(numCardsFound);
            mUi->finderLabel->setText(finderResult);
        }
        else
        {
            for (int i = 0; i < NUM_RESULT_WIDGETS; ++i)
            {
                mResultWidgets[i]->update();
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

void CCardSearchWidget::onCardDoubleClicked()
{
    CCardLabel *cardLabel = dynamic_cast<CCardLabel*>(QObject::sender());
    if (cardLabel)
    {
        const CCard &card = cardLabel->getCard();
        if (card.isValid())
        {
            emit cardSelected(card.getId());
            updateBoxHistory(mUi->nameBox);
            updateBoxHistory(mUi->skillBox);
        }
    }
}

void CCardSearchWidget::onCardDragged()
{
    updateBoxHistory(mUi->nameBox);
    updateBoxHistory(mUi->skillBox);
}

void CCardSearchWidget::showEvent(QShowEvent */*event*/)
{
    updateLayout();
}

void CCardSearchWidget::resizeEvent(QResizeEvent */*event*/)
{
    updateLayout();
}

void CCardSearchWidget::updateBoxHistory(QComboBox* box)
{
    if (box && box->lineEdit())
    {
        QString deckStr = box->lineEdit()->text().trimmed();;
        if (!deckStr.isEmpty())
        {
            int idx = box->findText(deckStr);
            if (idx > -1)
            {
                box->removeItem(idx);
            }
            box->insertItem(0, deckStr);
            box->setCurrentIndex(0);
        }
    }
}
