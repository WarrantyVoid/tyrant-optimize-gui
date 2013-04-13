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

void CCardFilterWidget::loadDefaultParameterSettings(QSettings &settings)
{
    mParameters.fetchFromSettings(settings);
    mParameters.updateUi(*mUi);
}

void  CCardFilterWidget::saveDefaultParameterSettings(QSettings &settings)
{
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

void CCardFilterWidget::updateOwnedCards()
{
    QClipboard* clip =  QApplication::clipboard();
    QString cardStr = clip->text();

    const CPathManager &pm = CPathManager::getPathManager();
    QString input = pm.getToolPath() + mOwnedCardsFile;
    QString output = pm.getToolPath() + "ownedcards_f.txt";
    CCardFilter filter;
    int numCards = filter.updateOwnedCards(cardStr, input);
    if (numCards > 0)
    {
        filter.execute(input, output, mParameters);
    }
    emit ownedCardsUpdated(numCards);
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
