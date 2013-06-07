#include "CDeckSaveWidget.h"
#include "ui_DeckSaveWidget.h"
#include "model/CDeckTable.h"
#include <QMessageBox>

CDeckSaveWidget::CDeckSaveWidget(const QString &deckName, QWidget *parent)
: QWidget(parent)
, mUi(new Ui::DeckSaveWidget)
, mResult(EDeckSaveCancel)
{
    mUi->setupUi(this);
    CDeckTable &decks = CDeckTable::getDeckTable();
    QStringList customDecks;
    decks.getCustomDecks(customDecks);
    customDecks.sort();
    mUi->deckSaveBox->updateHistory(customDecks);
    mUi->deckSaveBox->setEditText(deckName);

    connect(mUi->okButton, SIGNAL(clicked()),
        this, SLOT(okButtonPressed()));
    connect(mUi->cancelButton, SIGNAL(clicked()),
        this, SLOT(cancelButtonPressed()));
}

CDeckSaveWidget::~CDeckSaveWidget()
{
    delete mUi;
    mUi = 0;
}

QString CDeckSaveWidget::deckName() const
{
    if (mResult == EDeckSaveOk)
    {
        return mUi->deckSaveBox->currentText();
    }
    return "";
}

void CDeckSaveWidget::okButtonPressed()
{
    mResult = EDeckSaveCancel;
    const QString &customDeckName = mUi->deckSaveBox->currentText();
    if (!customDeckName.isEmpty())
    {
        CDeckTable &decks = CDeckTable::getDeckTable();
        const CDeck &existingDeck = decks.getDeckForName(customDeckName);
        if (existingDeck.isValid())
        {
            if (QMessageBox::question(
                this,
                "Overwrite existing Deck",
                "A deck with given name does already exist, overwrite?",
                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            {
                mResult = EDeckSaveOk;
            }
            else
            {
                return;
            }
        }
        else
        {
            mResult = EDeckSaveOk;
        }
    }
    parentWidget()->close();
}

void CDeckSaveWidget::cancelButtonPressed()
{
    mResult = EDeckSaveCancel;
    parentWidget()->close();
}
