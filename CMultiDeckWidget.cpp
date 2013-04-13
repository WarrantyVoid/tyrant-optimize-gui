#include "CMultiDeckWidget.h"
#include "CMultiDeckEditorWidget.h"
#include <QBoxLayout>

CMultiDeckWidget::CMultiDeckWidget(QWidget *parent)
: QWidget(parent)
, mDeckSourceWidget(0)
, mMultiDeckEditors()
, mAddButton(new QPushButton("Add Deck"))
, mCancelButton(new QPushButton("Cancel"))
, mOkButton(new QPushButton("Ok"))
, mNumberOfDecks(1)
{
    mAddButton->setIcon(QPixmap(":/img/add.png"));
    QBoxLayout *vLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    connect(
        mAddButton, SIGNAL(clicked()),
        this, SLOT(addDeck()));
    connect(
        mCancelButton, SIGNAL(clicked()),
        this, SLOT(declineDecks()));
    connect(
        mOkButton, SIGNAL(clicked()),
        this, SLOT(acceptDecks()));
    for (int i = 0; i < 10; ++i)
    {        
        mMultiDeckEditors[i] = new CMultiDeckEditorWidget(i);
        vLayout->addWidget(mMultiDeckEditors[i]);
        if (i >= mNumberOfDecks)
        {
            mMultiDeckEditors[i]->setVisible(false);
        }
    }
    for (int i = 0; i < 10; ++i)
    {
        connect(
            this, SIGNAL(numberOfDecksChanged(int)),
            mMultiDeckEditors[i], SLOT(updateBaseFraction(int)));
        for (int j = 0; j < 10; ++j)
        {
            if (j != i)
            {
                connect(
                    mMultiDeckEditors[i], SIGNAL(relativeFractionChanged(double, double)),
                    mMultiDeckEditors[j], SLOT(updateRelativeFraction(double, double)));
            }
        }
    }
    QBoxLayout *hLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    hLayout->addWidget(mAddButton);
    hLayout->addWidget(mOkButton);
    hLayout->addWidget(mCancelButton);    
    vLayout->addLayout(hLayout);
}

void CMultiDeckWidget::setDeckWidget(QLineEdit* editWidget)
{
    mDeckSourceWidget = editWidget;
}

void CMultiDeckWidget::resetEditors()
{
    mNumberOfDecks = 1;
    for (int i = 0; i < 10; ++i)
    {
        if (i >= mNumberOfDecks)
        {
            mMultiDeckEditors[i]->setVisible(false);
        }
        mMultiDeckEditors[i]->reset();
    }
}


void CMultiDeckWidget::declineDecks()
{
    emit decksUpdated(false);
}

void CMultiDeckWidget::acceptDecks()
{
    QStringList multiDeckStr;
    for (int i = 0; i < 10; ++i)
    {
        QString deckStr = "";
        if (mMultiDeckEditors[i])
        {
            deckStr = mMultiDeckEditors[i]->getResult();
        }
        if (!deckStr.isEmpty())
        {
            multiDeckStr.append(deckStr);
        }
    }
    if (mDeckSourceWidget)
    {
        mDeckSourceWidget->setText(multiDeckStr.join(";"));
    }
    emit decksUpdated(multiDeckStr.join(";"));
}

void CMultiDeckWidget::addDeck()
{
    if (mNumberOfDecks < 10)
    {
        mMultiDeckEditors[mNumberOfDecks]->setVisible(true);
        ++mNumberOfDecks;
        emit numberOfDecksChanged(mNumberOfDecks);
    }
}

void CMultiDeckWidget::showEvent(QShowEvent */*event*/)
{
    if (mDeckSourceWidget)
    {
        QStringList multiDeckStr = mDeckSourceWidget->text().split(";");
        resetEditors();
        double numDecks = qMin(10, multiDeckStr.size());
        double totalFractions = 0.0;
        double deckFractions[10];
        for (int i = 0; i < numDecks; ++i)
        {
            deckFractions[i] = 0.0;
            QStringList deckStr = multiDeckStr.at(i).split(":");
            if (deckStr.size() > 0)
            {
                mMultiDeckEditors[i]->updateDeck(deckStr.at(0));
                if (deckStr.size() > 1)
                {
                    bool ok(true);
                    deckFractions[i] = 100 * deckStr.at(1).toDouble(&ok);
                    if (ok)
                    {
                        totalFractions += deckFractions[i];
                    }
                    else
                    {
                        deckFractions[i] = 0.0;
                    }
                }
            }
            if (i > 0)
            {
                addDeck();
            }
        }
        for (int i = 0; i < numDecks; ++i)
        {
            double actualFraction = isDoubleEqual(totalFractions, 0.0)
                ? 100.0 / numDecks
                : 100.0 * deckFractions[i] / totalFractions;
            mMultiDeckEditors[i]->updateRelativeFraction(actualFraction);
        }
        parentWidget()->resize(300, 100);
        parentWidget()->adjustSize();
    }
}

bool CMultiDeckWidget::isDoubleEqual(double d1, double d2)
{
    return qAbs(d1 - d2) < 0.00000001;
}
