#include "CMultiDeckEditorWidget.h"
#include "ui_MultiDeckEditorWidget.h"
#include <QSortFilterProxyModel>
#include <QLineEdit>

CMultiDeckEditorWidget::CMultiDeckEditorWidget(int num, QWidget *parent)
: QWidget(parent)
, mUi(new Ui::MultiDeckEditorWidget)
, mCards(CCardTable::getCardTable())
, mNum(num)
, mTotalNum(1)
, mBaseFraction(100.0)
, mFractionOffset(0.0)
{
    mUi->setupUi(this);
}

CMultiDeckEditorWidget::~CMultiDeckEditorWidget()
{
    delete mUi;
}

QString CMultiDeckEditorWidget::getDeckId() const
{
    return mUi->deckEdit->currentText();
}

QString CMultiDeckEditorWidget::getResult() const
{
    if (isVisible() && !mUi->deckEdit->currentText().isEmpty())
    {
        if (mUi->deckFractionSpinBox->value() == 100)
        {
            return mUi->deckEdit->currentText();
        }
        else
        {
            return QString("%1:%2").arg(mUi->deckEdit->currentText()).arg(mUi->deckFractionSpinBox->value() * 0.01, 0, 'g', 2);
        }
    }
    return "";
}

void CMultiDeckEditorWidget::updateHistory()
{
    mUi->deckEdit->updateHistory();
}

void CMultiDeckEditorWidget::reset()
{
    mTotalNum = 1;
    mBaseFraction = 100.0;
    mFractionOffset = 0.0;
    mUi->deckFractionSpinBox->setValue(int(mBaseFraction + mFractionOffset + 0.5));
    mUi->deckEdit->setDeckId("");
}

void CMultiDeckEditorWidget::updateDeck(const QString &deck)
{
    mUi->deckEdit->setDeckId(deck);
}

void CMultiDeckEditorWidget::updateBaseFraction(int totalNum)
{
    mTotalNum = totalNum;
    mBaseFraction = 100.0 / totalNum;
    mFractionOffset = 0.0;
    mUi->deckFractionSpinBox->setValue(int(mBaseFraction + mFractionOffset + 0.5));
}

void CMultiDeckEditorWidget::updateRelativeFraction(double newFraction)
{
    mFractionOffset = newFraction - mBaseFraction;
    mUi->deckFractionSpinBox->setValue(int(mBaseFraction + mFractionOffset + 0.5));
}

void CMultiDeckEditorWidget::updateRelativeFraction(double oldFraction, double fractionDif)
{
    double ownRelation = isDoubleEqual(100.0, oldFraction)
        ? 1.0 / (mTotalNum - 1.0)
        :(mBaseFraction + mFractionOffset) / (100.0 - oldFraction);
    mFractionOffset += -(fractionDif * ownRelation);
    mUi->deckFractionSpinBox->setValue(int(mBaseFraction + mFractionOffset + 0.5));
}

void CMultiDeckEditorWidget::on_deckFractionSlider_sliderMoved(int value)
{
    double oldFraction = mBaseFraction + mFractionOffset;
    mFractionOffset = value - mBaseFraction;
    mUi->deckFractionSpinBox->setValue(int(mBaseFraction + mFractionOffset + 0.5));

    emit relativeFractionChanged(oldFraction, value - oldFraction);
}

bool CMultiDeckEditorWidget::isDoubleEqual(double d1, double d2)
{
    return qAbs(d1 - d2) < 0.00000001;
}
