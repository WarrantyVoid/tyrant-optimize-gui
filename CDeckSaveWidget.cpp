#include "CDeckSaveWidget.h"
#include "ui_DeckSaveWidget.h"
#include "model/CDeckTable.h"
#include <QMessageBox>

CDeckSaveWidget::CDeckSaveWidget(const CDeck &deck, EOptimizationMode optMode, QWidget *parent)
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
    mUi->deckSaveBox->setEditText(CDeckSaveWidget::buildDeckName(deck, optMode));

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

QString CDeckSaveWidget::buildDeckName(const CDeck &deck, EOptimizationMode optMode)
{
    QStringList deckName;

    // Prefix token
    QString prefix = "";
    bool isPve = false;
    switch(deck.getType())
    {
    case EMissionDeckType:
        prefix = QString("%1-%2").arg("pve").arg("mission");
        isPve = true;
        break;
    case ERaidDeckType:
        prefix = QString("%1-%2").arg("pve").arg("raid");
        isPve = true;
        break;
    case EQuestDeckType:
        prefix = QString("%1-%2").arg("pve").arg("quest");
        isPve = true;
        break;
    default:
        switch(optMode)
        {
        case EOptimizeWin:
                prefix = QString("%1-%2").arg("pvp").arg("att");
                break;
        case EOptimizeDefense:
                prefix = QString("%1-%2").arg("pvp").arg("def");
                break;
        default:
            break;
        }
    }
    if (!prefix.isEmpty())
    {
        deckName << prefix;
    }

    if (isPve)
    {
        QStringList nameTokens = deck.getName().toLower().split(" ");
        if (!nameTokens.isEmpty())
        {
            if (nameTokens.first().compare("mission") == 0)
            {
                nameTokens.removeFirst();
            }
            deckName << nameTokens;
        }

    }
    else
    {
        // Cards token
        QString commander = "unknown";
        QString legendary = "";
        QString unique = "";
        QString mostUsedCard = "";
        QMap<TCardId, QPair<const CCard*, int> > cardNumbers;
        QMap<ECardFaction, int> factionNumbers;
        int numUnique = 0;

        const TCardList &cards = deck.getCards();
        for (TCardList::const_iterator i = cards.begin(); i !=  cards.end(); ++i)
        {
            const CCard &curCard = *i;
            if (curCard.getType() == ECommanderType)
            {
                commander = CDeckSaveWidget::buildCardName(curCard);
            }
            else if (curCard.getRarity() == ELegendaryRarity)
            {
                legendary = CDeckSaveWidget::buildCardName(curCard);
            }
            else if (curCard.isUnique())
            {
                unique = CDeckSaveWidget::buildCardName(curCard);
                ++numUnique;
            }
            else
            {
                if (cardNumbers.contains(curCard.getId()))
                {
                    cardNumbers.find(curCard.getId()).value().second++;
                }
                else
                {
                    cardNumbers.insert(curCard.getId(), QPair<const CCard*, int>(&curCard, 1));
                }
            }
            if (curCard.getType() == EAssaultType)
            {
                if (factionNumbers.contains(curCard.getFaction()))
                {
                    factionNumbers.find(curCard.getFaction()).value()++;
                }
                else
                {
                    factionNumbers.insert(curCard.getFaction(), 1);
                }
            }
        }

        const CCard *maxUsedCard = &CCard::INVALID_CARD;
        int maxUsedCardNumber = 0;
        for (QMap<TCardId, QPair<const CCard*, int> >::const_iterator i = cardNumbers.begin(); i != cardNumbers.end(); ++i)
        {

            if (i.value().second > maxUsedCardNumber)
            {
                maxUsedCard = i.value().first;
                maxUsedCardNumber = i.value().second;
            }

        }

        ECardFaction maxUsedFaction = ENoFaction;
        int maxUsedFactionNumber = 0;
        int totalUsedFactionNumber = 0;
        for (QMap<ECardFaction, int>::const_iterator i = factionNumbers.begin(); i != factionNumbers.end(); ++i)
        {
            if (i.value() > maxUsedFactionNumber)
            {
                maxUsedFaction = i.key();
                maxUsedFactionNumber = i.value();
            }
            totalUsedFactionNumber += i.value();
        }

        if (maxUsedCardNumber > 2)
        {
            mostUsedCard = CDeckSaveWidget::buildCardName(*maxUsedCard);
        }
        else if (maxUsedFactionNumber > 2 && maxUsedFactionNumber >= (8 * totalUsedFactionNumber) / 10)
        {
            switch(maxUsedFaction)
            {
            case EImperialFaction: mostUsedCard = "imperial"; break;
            case EBloodthirstyFaction : mostUsedCard = "bloodthirsty"; break;
            case EXenoFaction: mostUsedCard = "xeno"; break;
            case ERighteousFaction: mostUsedCard = "righteous"; break;
            case ERaiderFaction: mostUsedCard = "raider"; break;
            default: break;
            }
        }
        else
        {
            mostUsedCard = "rainbow";
        }

        if (!commander.isEmpty())
        {
            deckName << commander;
        }
        if (!mostUsedCard.isEmpty())
        {
            deckName << mostUsedCard;
        }
        if (!legendary.isEmpty())
        {
            deckName << legendary;
        }
        else if (!unique.isEmpty() && numUnique == 1)
        {
            deckName << unique;
        }
    }
    return deckName.join("-");
}

QString CDeckSaveWidget::buildCardName(const CCard &card)
{
    QString name = "unknown";
    QStringList nameTokens = card.getName().split(QRegExp("\\s|\\,|\\*"), QString::SkipEmptyParts);
    if (!nameTokens.empty())
    {
        name = nameTokens.at(0);
        if (nameTokens.size() > 1)
        {
            name = "";
            for (int i = 0; i < nameTokens.size(); ++i)
            {
                name += nameTokens.at(i).left(1);
            }
        }
    }
    return name.toLower();
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
