#include "CCardTable.h"
#include "CPictureDownload.h"
#include "CGlobalConfig.h"
#include <QFileInfo>
#include <QDateTime>
#include <QXmlSimpleReader>
#include <QNetworkRequest>
#include <QNetworkProxyFactory>
#include <QUrl>
#include "xml/CCardsXmlParser.h"
#include "xml/CQuestsXmlParser.h"
#include "xml/CAchievementsXmlParser.h"

CCardTable* CCardTable::CARD_TABLE = 0;

CCardTable::CCardTable()
: mSkillIdMap()
, mCardSetIdMap()
, mCardIdMap()
, mCardNameMap()
, mNetManager(new QNetworkAccessManager())
, mPictureDownloads()
, mDataDownloads()
, mDataDownloadResults()
, mBattlegrounds()
, mAchievements()
, mTotalDownloads(0)
, mFinishedDownloads(0)
{
    initData();  
    QNetworkProxyFactory::setUseSystemConfiguration(true);
}

CCardTable::~CCardTable()
{
    QHashIterator<unsigned int, CCard*> i(mCardIdMap);
    while (i.hasNext())
    {
        i.next();
        delete i.value();
    }
    mCardIdMap.clear();
    mCardNameMap.clear();
}

CCardTable& CCardTable::getCardTable()
{
    if (!CARD_TABLE)
    {
        CARD_TABLE = new CCardTable();
    }
    return *CARD_TABLE;
}

const CSkill& CCardTable::getSkillForId(const QString id) const
{
    QHash<QString, CSkill>::const_iterator i = mSkillIdMap.find(id);
    if (i != mSkillIdMap.end() && i.key() == id)
    {
        return i.value();
    }
    return CSkill::INVALID_SKILL;
}

const QString CCardTable::getPictureForCardSet(ECardSet cardSet) const
{
    QHash<ECardSet, QString>::const_iterator i = mCardSetIdMap.find(cardSet);
    if (i != mCardSetIdMap.end() && i.key() == cardSet)
    {
        return i.value();
    }
    return "";
}

const CCard& CCardTable::getCardForId(unsigned int id) const
{
    QHash<unsigned int, CCard*>::const_iterator i = mCardIdMap.find(id);
    if (i != mCardIdMap.end() && i.key() == id)
    {
        return *i.value();
    }
    return CCard::INVALID_CARD;
}

const CCard& CCardTable::getCardForName(const QString &name) const
{
    QHash<QString, CCard*>::const_iterator i = mCardNameMap.find(name);
    if (i != mCardNameMap.end() && i.key() == name)
    {
        return *i.value();
    }
    return CCard::INVALID_CARD;
}

void CCardTable::searchCards(const ICardCheck &search, QList<CCard*> &cards, int maxHits) const
{
    cards.clear();
    int dummy(0);
    for (QHash<unsigned int, CCard*>::const_iterator i = mCardIdMap.begin(); i != mCardIdMap.end(); ++i)
    {
        if (search.checkCard(*i.value(), dummy))
        {
            cards.push_back(i.value());
            if (cards.size() == maxHits)
            {
                return;
            }
        }
    }
}

bool CCardTable::isCardOwned(CCard *card)
{
    return (card && mOwnedCardMap.contains(card->getId()));
}

void CCardTable::setOwnedCards(const QList<TOwnedCard> &ownedCards)
{
    mOwnedCardMap.clear();
    for ( QList<TOwnedCard>::const_iterator i = ownedCards.begin(); i != ownedCards.end(); ++i)
    {
        mOwnedCardMap.insert((*i).first.getId());
    }
    emit ownedCardsUpdated();
}

void  CCardTable::updateData()
{
    if (mDataDownloads.isEmpty())
    {
        if (mPictureDownloads.isEmpty())
        {
            mDataDownloadResults.clear();
            const CGlobalConfig& pm = CGlobalConfig::getCfg();

            CDownload* cardsDownload = new CDownload("http://kg.tyrantonline.com/assets/cards.xml", pm.getToolPath() + "cards.xml", true);
            CDownload* raidsDownload = new CDownload("http://kg.tyrantonline.com/assets/raids.xml", pm.getToolPath() + "raids.xml", true);
            CDownload* missionsDownload = new CDownload("http://kg.tyrantonline.com/assets/missions.xml", pm.getToolPath() + "missions.xml", true);
            CDownload* questsDownload = new CDownload("http://kg.tyrantonline.com/assets/quests.xml", pm.getToolPath() + "quests.xml", true);
            CDownload* achievementsDownload = new CDownload("http://kg.tyrantonline.com/assets/achievements.xml", pm.getToolPath() + "achievements.xml", true);

            mDataDownloads.enqueue(cardsDownload);
            mDataDownloads.enqueue(raidsDownload);
            mDataDownloads.enqueue(missionsDownload);
            mDataDownloads.enqueue(questsDownload);
            mDataDownloads.enqueue(achievementsDownload);

            connect(
                mDataDownloads.head(), SIGNAL(downloadHandled()),
                this, SLOT(processNextDataDownload()));
            mDataDownloads.head()->startDownload(mNetManager);
        }
        else
        {
            mDataDownloadResults.clear();
            mDataDownloadResults.push_back("Please try again when picture download is done.");
            emit dataUpdated(mDataDownloadResults);
        }
    }
}

const CBattleground& CCardTable::getBattlegroundForId(unsigned int id) const
{
    for(QList<CBattleground>::const_iterator i = mBattlegrounds.begin(); i != mBattlegrounds.end(); ++i)
    {
        if ((*i).getId() == id)
        {
            return *i;
        }
    }
    return CBattleground::INVALID_BATTLEGROUND;
}

const QList<CBattleground>& CCardTable::getBattlegrounds() const
{
    return mBattlegrounds;
}

const QList<CAchievement>& CCardTable::getAchievements() const
{
    return mAchievements;
}

void CCardTable::processNextPictureDownload()
{
    if (!mPictureDownloads.isEmpty())
    {
        ++mFinishedDownloads;
        emit downloadProgress(mFinishedDownloads, mTotalDownloads);
        CPictureDownload* download = mPictureDownloads.dequeue();
        if (download)
        {
            const CCard &card = download->getPictureMetaData();
            if (card.isValid() && !mCardIdMap.contains(card.getId()))
            {
                CCard* c = new CCard(card);
                mCardIdMap.insert(card.getId(), c);
                if (!card.getSet() == EUnknownSet && !mCardNameMap.contains(card.getName()))
                {
                    mCardNameMap.insert(card.getName(), c);
                }
            }
            delete download;
            download = 0;
        }
    }
    if (!mPictureDownloads.isEmpty())
    {
        connect(
            mPictureDownloads.head(), SIGNAL(downloadHandled()),
            this, SLOT(processNextPictureDownload()));
        mPictureDownloads.head()->startDownload(mNetManager);
    }
}

void CCardTable::processNextDataDownload()
{
    if (!mDataDownloads.isEmpty())
    {
        CDownload* download = mDataDownloads.dequeue();
        QFileInfo fileInfo(download->fileName());
        QString action = "";
        switch (download->state())
        {
        case EDownloadNotStarted: action = "Not started"; break;
        case EDownloadStarted: action = "Started"; break;
        case EDownloadFailed: action = "Failed to download"; break;
        case EDownloadSuccessful: action = "Already up-to-date"; break;
        case EDownloadStored: action = "Updated"; break;
        case EDownloadStoreFailed: action = "Failed to write"; break;
        }
        QString result = QString("<tr><td>%1</td><td>:</td><td>%2</td></tr>").arg(fileInfo.fileName()).arg(action);
        mDataDownloadResults.append(result);
        delete download;
        download = 0;
    }
    if (mDataDownloads.isEmpty())
    {
        mDataDownloadResults.prepend("<table>");
        mDataDownloadResults.append("</table>");
        emit dataUpdated(mDataDownloadResults);
        initData();
    }
    else
    {
        connect(
            mDataDownloads.head(), SIGNAL(downloadHandled()),
            this, SLOT(processNextDataDownload()));
        mDataDownloads.head()->startDownload(mNetManager);
    };
}

void CCardTable::processSkill(const CSkill& skill)
{
    if (skill.isValid() && !mSkillIdMap.contains(skill.getId()))
    {
        mSkillIdMap.insert(skill.getId(), skill);
    }
}

void CCardTable::processCard(const CCard &card)
{
    if (card.isValid() && !mCardIdMap.contains(card.getId()))
    {
        //http://cdn.tyrantonline.com/warhawk/images/cardname.jpg
        //http://www.facebook.com/media/set/?set=a.240926515984717.57623.235197086557660&type=1
        QString pictureFileName(CGlobalConfig::getCfg().getPicturePath() + card.getPicture());
        QFile pictureFile(pictureFileName);
        if (pictureFile.exists())
        {
            CCard* c = new CCard(card);
            mCardIdMap.insert(card.getId(), c);
            if (!card.getSet() == EUnknownSet && !mCardNameMap.contains(card.getName()))
            {
                mCardNameMap.insert(card.getName(), c);
            }
        }
        else
        {
            QString pictureUrlName(CGlobalConfig::getCfg().getOnlinePicturePath() + card.getPicture());
            CPictureDownload *download = new CPictureDownload(pictureUrlName, pictureFileName);
            download->setPictureMetaData(card);
            mPictureDownloads.enqueue(download);
        }

    }
}

void CCardTable::processCardSet(ECardSet cardSet, const QString &picture)
{
    if (cardSet != EUnknownSet && !mCardSetIdMap.contains(cardSet) && !picture.isEmpty())
    {
        mCardSetIdMap.insert(cardSet, picture);
    }
}

void CCardTable::processBattleground(const CBattleground& battleground)
{
    if (battleground.isValid())
    {
        mBattlegrounds.push_back(battleground);
        //Download battleground icons
        //QString pictureFileName("E:\\Eigene Dateien\\Programme\\[Projects]\\tyrant_optimize_gui\\tyrant\\" + battleground.getPicture());
        //QString pictureUrlName(CPathManager::getPathManager().getOnlinePicturePath() + battleground.getPicture());
        //CPictureDownload *download = new CPictureDownload(pictureUrlName, pictureFileName);
        //mPictureDownloads.enqueue(download);
    }
}

void CCardTable::processAchievement(const CAchievement& achievement)
{
    if (achievement.isValid())
    {
        mAchievements.push_back(achievement);
    }
}

void CCardTable::initData()
{
    // Cleanup
    mSkillIdMap.clear();
    QHashIterator<unsigned int, CCard*> i(mCardIdMap);
    while (i.hasNext())
    {
        i.next();
        delete i.value();
    }
    mCardIdMap.clear();
    mCardNameMap.clear();
    while (!mPictureDownloads.isEmpty())
    {
        CPictureDownload* curDown = mPictureDownloads.dequeue();
        if (curDown)
        {
            delete curDown;
            curDown = 0;
        }
    }
    while (!mDataDownloads.isEmpty())
    {
        CDownload* curDown = mDataDownloads.dequeue();
        if (curDown)
        {
            delete curDown;
            curDown = 0;
        }
    }
    mDataDownloadResults.clear();
    mBattlegrounds.clear();
    mAchievements.clear();
    mTotalDownloads = 0;
    mFinishedDownloads = 0;

    const CGlobalConfig& pm = CGlobalConfig::getCfg();

    //load cards
    QFile cardFile(pm.getToolPath() + "cards.xml");
    if (cardFile.open(QIODevice::ReadOnly))
    {
        CCardsXmlParser cardXmlParser;
        connect(
            &cardXmlParser, SIGNAL(cardParsed(const CCard&)),
            this, SLOT(processCard(const CCard&)));
        connect(
            &cardXmlParser, SIGNAL(skillParsed(const CSkill&)),
            this, SLOT(processSkill(const CSkill&)));
        connect(
            &cardXmlParser, SIGNAL(cardSetParsed(ECardSet, const QString &)),
            this, SLOT(processCardSet(ECardSet, const QString &)));

        QXmlInputSource cardXml(&cardFile);
        QXmlSimpleReader cardReader;
        cardReader.setContentHandler(&cardXmlParser);
        cardReader.parse(cardXml);
        cardFile.close();
    }

    //load battle grounds
    QFile questFile(pm.getToolPath() + "quests.xml");
    if (questFile.open(QIODevice::ReadOnly))
    {
        CQuestsXmlParser questXmlParser;
        connect(
            &questXmlParser, SIGNAL(battlegroundParsed(const CBattleground&)),
            this, SLOT(processBattleground(const CBattleground&)));

        QXmlInputSource questXml(&questFile);
        QXmlSimpleReader questReader;
        questReader.setContentHandler(&questXmlParser);
        questReader.parse(questXml);
        questFile.close();
    }

    // load achievements
    QFile achievementFile(pm.getToolPath() + "achievements.xml");
    if (achievementFile.open(QIODevice::ReadOnly))
    {
        CAchievementsXmlParser achievementXmlParser;
        connect(
            &achievementXmlParser, SIGNAL(achievementParsed(const CAchievement&)),
            this, SLOT(processAchievement(const CAchievement&)));

        QXmlInputSource achievementXml(&achievementFile);
        QXmlSimpleReader achievementReader;
        achievementReader.setContentHandler(&achievementXmlParser);
        achievementReader.parse(achievementXml);
        achievementFile.close();
    }

    // Start downloads
    if (!mPictureDownloads.isEmpty())
    {
        mTotalDownloads = mPictureDownloads.size();

        connect(
            mPictureDownloads.head(), SIGNAL(downloadHandled()),
            this, SLOT(processNextPictureDownload()));
        mPictureDownloads.head()->startDownload(mNetManager);
    }
}
