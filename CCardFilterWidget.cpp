#include "CCardFilterWidget.h"
#include "CPathManager.h"
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
}

void  CCardFilterWidget::saveParameterSettings(QSettings &settings)
{
    mParameters.fetchFromUi(*mUi);
    mParameters.updateSettings(settings);
}

void CCardFilterWidget::scanOwnedCards(QStringList &result) const
{
    result.clear();
    const CPathManager &pm = CPathManager::getPathManager();
    QDir dir(pm.getToolPath());
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
        const CPathManager &pm = CPathManager::getPathManager();
        QFileInfo fileInfo(pm.getToolPath() + mOwnedCardsFile);
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
    const CPathManager &pm = CPathManager::getPathManager();
    mOwnedCardsFile = fileName;
    QString filePath = pm.getToolPath() + mOwnedCardsFile;
    CCardFilter filter;
    QList<TOwnedCard> ownedCards;
    SOwnedCardStatistics oldStats = filter.readOwnedCardsFromFile(filePath, ownedCards);
    SOwnedCardStatistics newStats = filter.readOwnedCardsFromClipboard(clip->text(), ownedCards);
    if (newStats.numCards > 0)
    {
        newStats = filter.writeOwnedCardsToFile(filePath, ownedCards);
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
    const CPathManager &pm = CPathManager::getPathManager();
    mOwnedCardsFile = fileName;
    mOwnedCardsTime = QFileInfo(pm.getToolPath() + fileName).lastModified();
    executeFilter();
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
    const CPathManager &pm = CPathManager::getPathManager();
    QString input = pm.getToolPath() + mOwnedCardsFile;
    QString output = pm.getToolPath() + "ownedcards_f.txt";
    CCardFilter filter;
    filter.execute(input, output, mParameters);
}
