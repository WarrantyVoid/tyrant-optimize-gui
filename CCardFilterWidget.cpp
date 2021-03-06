#include "CCardFilterWidget.h"
#include "CGlobalConfig.h"
#include "filter/CCardFilter.h"
#include "ui_CardFilterWidget.h"
#include <QApplication>
#include <QClipboard>
#include <QDir>

CCardFilterWidget::CCardFilterWidget(QWidget *parent)
: QWidget(parent)
, mUi(new Ui::CardFilterWidget)
, mParameters()
, mOwnedCardsFile("ownedcards.txt")
{
    mUi->setupUi(this);

    connect(
        mUi->cancelButton, SIGNAL(clicked()),
        this, SLOT(declineFilter()));

    connect(
        mUi->okButton, SIGNAL(clicked()),
        this, SLOT(acceptFilter()));
    connect(
        mUi->resetButton, SIGNAL(clicked()),
        this, SLOT(resetFilter()));
}

CCardFilterWidget::~CCardFilterWidget()
{
    delete mUi;
}

void CCardFilterWidget::loadParameterSettings(QSettings &settings)
{
    mParameters.fetchFromSettings(settings);
    mParameters.updateUi(*mUi);
}

void  CCardFilterWidget::saveParameterSettings(QSettings &settings)
{
    mParameters.fetchFromUi(*mUi);
    mParameters.updateSettings(settings);
}

void CCardFilterWidget::scanOwnedCards(QStringList &result) const
{
    result.clear();
    const CGlobalConfig &cfg = CGlobalConfig::getCfg();
    QDir dir(cfg.getToolPath());
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::Readable | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    dir.setNameFilters(QStringList("*.txt"));
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QString fileName = list.at(i).fileName();
        if (fileName.compare("ownedcards_f.txt") != 0
            && fileName.compare("custom.txt") != 0 )
        {
            result.push_back(list.at(i).fileName());
        }
    }
}

bool CCardFilterWidget::hasOwnedCardsChanged(const QString &fileName) const
{
    if (fileName.compare(mOwnedCardsFile) == 0)
    {
        const CGlobalConfig &cfg = CGlobalConfig::getCfg();
        QFileInfo fileInfo(cfg.getToolPath() + mOwnedCardsFile);
        if (fileInfo.exists() && fileInfo.lastModified() == mOwnedCardsTime)
        {
            return false;
        }
    }
    return true;
}

void CCardFilterWidget::updateOwnedCardsFile(const QString &fileName)
{
    QClipboard* clip =  QApplication::clipboard();
    const CGlobalConfig &cfg = CGlobalConfig::getCfg();
    mOwnedCardsFile = fileName;
    QString filePath = cfg.getToolPath() + mOwnedCardsFile;
    CCardFilter filter;
    QList<TOwnedCard> ownedCards;
    SOwnedCardStatistics oldStats = filter.readOwnedCardsFromFile(filePath, ownedCards);
    SOwnedCardStatistics newStats = filter.readOwnedCardsFromClipboard(clip->text(), ownedCards);
    if (newStats.numCards > 0)
    {
        newStats = filter.writeOwnedCardsToFile(filePath, ownedCards);

        // Updated owned status
        CCardTable &cards = CCardTable::getCardTable();
        cards.setOwnedCards(ownedCards, ownedCards);
    }

    QStringList updateResult;
    if (newStats.numCards > 0)
    {
        updateResult << "<p>Updated 'ownedcards.txt' from clipboard:</p>";
        updateResult << "<table>";
        updateResult << QString("<tr><td>#Cards:</td><td>%1</td><td>-></td><td>%2</td></tr>")
                        .arg(oldStats.numCards).arg(newStats.numCards);
        updateResult << QString("<tr><td>#Copies:</td><td>%1</td><td>-></td><td>%2</td></tr>")
                        .arg(oldStats.numCopies).arg(newStats.numCopies);
        updateResult.append("</table>");
    }
    else
    {
         updateResult << "Could not update 'ownedcards.txt' from clipboard. Please copy card export string from fansite into clipboard first.";
    }

    emit ownedCardsUpdated(updateResult);
}

void CCardFilterWidget::setOwnedCardsFile(const QString &fileName)
{
    const CGlobalConfig &cfg = CGlobalConfig::getCfg();
    mOwnedCardsFile = fileName;
    mOwnedCardsTime = QFileInfo(cfg.getToolPath() + fileName).lastModified();
    executeFilter();
}

void CCardFilterWidget::setDeckBlockage(const CDeck &deck, bool isBlocked)
{
    mParameters.setDeckBlockage(deck, isBlocked);
    mParameters.updateUi(*mUi);
    mOwnedCardsTime = QDateTime();
}

void CCardFilterWidget::setCardBlackListStatus(const CCard &card, bool isBlack)
{
    mParameters.setCardBlackListStatus(card, isBlack);
    mParameters.updateUi(*mUi);
    mOwnedCardsTime = QDateTime();

    pushParametersToModel();
}

void CCardFilterWidget::setCardWhiteListStatus(const CCard &card, bool isWhite)
{
    mParameters.setCardWhiteListStatus(card, isWhite);
    mParameters.updateUi(*mUi);
    mOwnedCardsTime = QDateTime();

    pushParametersToModel();
}

void CCardFilterWidget::createOwnedAllCardsFile()
{
    const CGlobalConfig &cfg = CGlobalConfig::getCfg();
    CCardTable &cardTable = CCardTable::getCardTable();
    QList<CCard*> cards;
    cardTable.searchCards(CardCheckAcceptAll(), cards);
    QList<TOwnedCard> allCards;
    for (QList<CCard*>::iterator i = cards.begin(); i != cards.end(); ++i)
    {
        allCards.push_back(TOwnedCard(**i, 99));
    }
    CCardFilter filter;
    filter.writeOwnedCardsToFile(cfg.getToolPath() + "ownedcards_all.txt", allCards);
}

void CCardFilterWidget::declineFilter()
{
    mParameters.updateUi(*mUi);
    emit filterUpdated(false);
}

void CCardFilterWidget::acceptFilter()
{
    mOwnedCardsTime = QDateTime();
    if (mParameters.fetchFromUi(*mUi))
    {
        emit filterUpdated(true);
    }
    else
    {
        mParameters.updateUi(*mUi);
    }

    pushParametersToModel();
}

void CCardFilterWidget::resetFilter()
{
    mParameters.reset();
    mParameters.updateUi(*mUi);
    mOwnedCardsTime = QDateTime();
}

void CCardFilterWidget::executeFilter()
{
    const CGlobalConfig &cfg = CGlobalConfig::getCfg();
    CCardFilter filter;
    QList<TOwnedCard> originalCards;
    QList<TOwnedCard> filteredCards;
    filter.readOwnedCardsFromFile(cfg.getToolPath() + mOwnedCardsFile, originalCards);
    filter.execute(originalCards, filteredCards, mParameters);
    filter.writeOwnedCardsToFile(cfg.getToolPath() + "ownedcards_f.txt", filteredCards);

    // Updated owned status
    CCardTable &cards = CCardTable::getCardTable();
    cards.setOwnedCards(originalCards, filteredCards);

    pushParametersToModel();
}

void CCardFilterWidget::pushParametersToModel()
{
    // Update black & white list status
    CCardTable &cards = CCardTable::getCardTable();
    QStringList blackList;
    QStringList whiteList;
    mParameters.getBlackList(blackList);
    mParameters.getWhiteList(whiteList);
    cards.setListedCards(blackList, whiteList);
}
