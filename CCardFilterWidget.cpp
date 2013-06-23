#include "CCardFilterWidget.h"
#include "CGlobalConfig.h"
#include "CCardFilter.h"
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
}

CCardFilterWidget::~CCardFilterWidget()
{
    delete mUi;
}

void CCardFilterWidget::loadParameterSettings(QSettings &settings)
{
    mParameters.fetchFromSettings(settings);
    mParameters.updateUi(*mUi);

    const CGlobalConfig &cfg = CGlobalConfig::getCfg();
    CCardFilter filter;
    QList<TOwnedCard> ownedCards;
    filter.readOwnedCardsFromFile(cfg.getToolPath() + mOwnedCardsFile, ownedCards);
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
        cards.setOwnedCards(ownedCards);
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

void CCardFilterWidget::setCardsBlackListed(const QStringList &cards, bool toBlack)
{
    mParameters.setCardsBlackListed(cards, toBlack);
    mParameters.updateUi(*mUi);
}

void CCardFilterWidget::declineFilter()
{
    mParameters.updateUi(*mUi);
    emit filterUpdated(false);
}

void CCardFilterWidget::acceptFilter()
{
    mParameters.fetchFromUi(*mUi);
    mOwnedCardsTime = QDateTime();
    emit filterUpdated(true);
}

void CCardFilterWidget::executeFilter()
{
    const CGlobalConfig &cfg = CGlobalConfig::getCfg();
    QString input = cfg.getToolPath() + mOwnedCardsFile;
    QString output = cfg.getToolPath() + "ownedcards_f.txt";
    CCardFilter filter;
    QList<TOwnedCard> originalCards;
    QList<TOwnedCard> filteredCards;
    filter.readOwnedCardsFromFile(input, originalCards);
    filter.execute(originalCards, filteredCards, mParameters);
    filter.writeOwnedCardsToFile(output, filteredCards);

    // Updated owned status
    CCardTable &cards = CCardTable::getCardTable();
    cards.setOwnedCards(originalCards);
}
