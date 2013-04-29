#include "CCardTable.h"
#include "CPictureDownload.h"
#include "CPathManager.h"
#include <QFileInfo>
#include <QDateTime>
#include <QXmlSimpleReader>
#include <QNetworkRequest>
#include <QUrl>
#include "xml/CCardsXmlParser.h"
#include "xml/CMissionsXmlParser.h"
#include "xml/CRaidsXmlParser.h"
#include "xml/CQuestsXmlParser.h"
#include "xml/CAchievementsXmlParser.h"

CCardTable* CCardTable::CARD_TABLE = 0;
const QString CCardTable::BASE_64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const QChar CCardTable::BASE_64_CHAR_EXT = '-';

CCardTable::CCardTable()
: mSkillIdMap()
, mCardSetIdMap()
, mCardIdMap()
, mCardNameMap()
, mDeckNameMap()
, mNetManager(new QNetworkAccessManager())
, mPictureDownloads()
, mDataDownloads()
, mDataDownloadResults()
, mCustomDecks()
, mRaidDecks()
, mBattlegrounds()
, mAchievements()
, mTotalDownloads(0)
, mFinishedDownloads(0)
{
    initData();
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
    for (QHash<QString, CCard*>::const_iterator i = mCardNameMap.begin(); i != mCardNameMap.end(); ++i)
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

void  CCardTable::updateData()
{
    if (mDataDownloads.isEmpty())
    {
        if (mPictureDownloads.isEmpty())
        {
            mDataDownloadResults.clear();
            const CPathManager& pm = CPathManager::getPathManager();

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

bool CCardTable::addCustomDeck(CDeck &customDeck)
{
    bool newDeckCreated = true;
    QHash<QString, CDeck>::iterator iDeck = mDeckNameMap.find(customDeck.getName());
    if (iDeck != mDeckNameMap.end() && iDeck.key() == customDeck.getName())
    {
        iDeck.value() = customDeck;
        newDeckCreated = false;
    }
    else
    {
        mDeckNameMap.insert(customDeck.getName(), customDeck);
        mCustomDecks.push_back(customDeck.getName()); 
    }

    QFile customDeckFile(CPathManager::getPathManager().getToolPath() + "custom.txt");
    if (customDeckFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        mCustomDecks.sort();
        QTextStream out(&customDeckFile);
        const QStringList &customDecks = getCustomDecks();
        int deckNameWidth = 18;
        for (QStringList::const_iterator iDeck = customDecks.begin(); iDeck != customDecks.end(); ++iDeck)
        {
            if ((*iDeck).length() > deckNameWidth)
            {
                deckNameWidth = (*iDeck).length();
            }
        }
        for (QStringList::const_iterator iDeck = customDecks.begin(); iDeck != customDecks.end(); ++iDeck)
        {
            CDeck deck;
            if (nameToDeck(*iDeck, deck))
            {
                QString deckId = QString("%1").arg(deck.getName(), -deckNameWidth, QChar(' '));
                const QList<CCard> &cards = deck.getCards();
                out << deckId << ":";
                for (int iCard = 0; iCard < cards.size(); ++iCard)
                {
                    int num = 1;
                    const CCard &curCard = cards[iCard];
                    while (iCard < cards.size() - 1 && curCard.getId() == cards.at(iCard + 1).getId())
                    {
                        ++iCard;
                        ++num;
                    }
                    QStringList cardSplitComma = curCard.getName().split(QRegExp("\\,"), QString::SkipEmptyParts);
                    if (cardSplitComma.size() > 1)
                    {
                        out << " " << cardSplitComma.at(0) << " [" << curCard.getId() << "]";
                    }
                    else
                    {
                        out << " " << curCard.getName();
                    }
                    if (num > 1)
                    {
                        out << " #" << num;
                        num = 0;
                    }
                    if (iCard < deck.getCards().size() - 1)
                    {
                        out << ",";
                    }
                }
                out << "\n";
            }
        }
    }
    else
    {
        newDeckCreated = false;
    }
    return newDeckCreated;
}

const QStringList& CCardTable::getCustomDecks()
{
    return mCustomDecks;
}

const QStringList& CCardTable::getRaidDecks()
{
    return mRaidDecks;
}

const QList<CBattleground>& CCardTable::getBattlegrounds()
{
    return mBattlegrounds;
}

const QList<CAchievement>& CCardTable::getAchievements()
{
    return mAchievements;
}

bool CCardTable::deckToHash(const CDeck &deck, QString &hashStr) const
{
    hashStr.clear();
    const QList<CCard> &cards = deck.getCards();
    unsigned int lastId(0);
    unsigned int num(0);
    for (QList<CCard>::ConstIterator iCard = cards.begin(); iCard != cards.end(); ++iCard)
    {
        unsigned int id = iCard->getId();
        if (id == lastId)
        {
            ++num;
        }
        else
        {
            if (num > 1)
            {
                unsigned int index1 = 0x3F & ((num + 4000) >> 6);
                unsigned int index2 = 0x3F & (num + 4000);
                hashStr.append(BASE_64_CHARS[index1]);
                hashStr.append(BASE_64_CHARS[index2]);
            }
            lastId = id;
            num = 1;
            if (id > 4000)
            {
                hashStr.append(BASE_64_CHAR_EXT);
                id -= 4000;
            }
            unsigned int index1 = 0x3F & (id >> 6);
            unsigned int index2 = 0x3F & id;
            hashStr.append(BASE_64_CHARS[index1]);
            hashStr.append(BASE_64_CHARS[index2]);
        }
    }
    if (num > 1)
    {
        unsigned int index1 = 0x3F & ((num + 4000) >> 6);
        unsigned int index2 = 0x3F & (num + 4000);
        hashStr.append(BASE_64_CHARS[index1]);
        hashStr.append(BASE_64_CHARS[index2]);
    }
    return hashStr.length() > 1;
}

bool CCardTable::hashToDeck(const QString &hashStr, CDeck &deck) const
{
    QList<unsigned int> ids;
    int maxIds = 11;
    int lastIndex = -1;
    unsigned int lastId = 0;
    unsigned int extId = 0;
    
    for (QString::ConstIterator i = hashStr.begin(); i != hashStr.end() && ids.size() <= maxIds; ++i)
    {
        if (*i == BASE_64_CHAR_EXT)
        {
            extId = 4000;
        }
        else
        {
            int curIndex = BASE_64_CHARS.indexOf(*i);
            if (curIndex > -1)
            {
                if(lastIndex > -1)
                {
                    unsigned int curId = (lastIndex << 6) + curIndex;
                    if (curId < 4001)
                    {
                        ids.push_back(curId + extId);
                        lastId = curId + extId;
                    }
                    else for (unsigned int j = 0; j < curId - 4001 && ids.size() <= maxIds; ++j)
                    {
                        ids.push_back(lastId);
                    }
                    lastIndex = -1;
                    extId = 0;
                }
                else
                {
                    lastIndex = curIndex;
                }
            }
            else
            {
                return false;
            }
        }
    }
    
    deck.setName(hashStr);
    deck.clearCards();
    for (QList<unsigned int>::ConstIterator iId = ids.begin(); iId != ids.end(); ++iId)
    {
        QHash<unsigned int, CCard*>::const_iterator iCard = mCardIdMap.find(*iId);
        if (iCard != mCardIdMap.end() && iCard.key() == *iId)
        {
            deck.addCard(*iCard.value());
        }
        else
        {
            return false;
        }
    }
    return deck.isValid();
}

bool CCardTable::nameToDeck(const QString &deckName, CDeck &deck) const
{
    deck.setName(deckName);
    deck.clearCards();
    QHash<QString, CDeck>::const_iterator i = mDeckNameMap.find(deckName);
    if (i != mDeckNameMap.end() && i.key() == deckName)
    {
        deck = i.value();
        return true;
    }
    return deck.isValid();
}

bool CCardTable::strToDeck(const QString &deckStr, CDeck &deck) const
{
    QStringList customDeckTokens = deckStr.split(QRegExp("\\,"), QString::SkipEmptyParts);
    QStringList customDeckCards;
    deck.setName(deckStr);
    deck.clearCards();
    if (customDeckTokens.size() > 0)
    {
        for (int iToken = 0;  iToken < customDeckTokens.size(); ++iToken)
        {
            int num = 1;
            QString curCardStr = customDeckTokens.at(iToken).trimmed();
            CCard curCard = getCardForName(curCardStr);

            if (iToken > 0 && !curCard.isValid())
            {
                QStringList cardSplitNum = curCardStr.split(QRegExp("\\#"), QString::SkipEmptyParts);
                if (cardSplitNum.size() > 1)
                {
                    curCardStr = cardSplitNum.at(0).trimmed();
                    curCard = getCardForName(curCardStr);
                    bool ok(true);
                    num = cardSplitNum.at(1).toInt(&ok);
                }
            }

            if (!curCard.isValid())
            {
                QStringList cardSplitId = curCardStr.split(QRegExp("\\[|\\]"), QString::SkipEmptyParts);
                if (cardSplitId.size() > 1)
                {
                    bool ok(true);
                    unsigned int cardId = cardSplitId.at(1).toUInt(&ok);
                    curCard = getCardForId(cardId);
                }
            }

            if (curCard.isValid() && num > 0)
            {                
                for(int iNum = 0; iNum < num && customDeckCards.size() < 12; ++iNum)
                {
                    deck.addCard(curCard);
                }
            }
        }
    }
    return deck.isValid();
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
        QString result = QString("'%1': \t%2").arg(fileInfo.fileName()).arg(action);
        mDataDownloadResults.append(result);
        delete download;
        download = 0;
    }
    if (mDataDownloads.isEmpty())
    {
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
        QString pictureFileName(CPathManager::getPathManager().getPicturePath() + card.getPicture());
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
            QString pictureUrlName(CPathManager::getPathManager().getOnlinePicturePath() + card.getPicture());
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

void CCardTable::processDeck(const QString &deckName, EDeckType type, const QList<unsigned int> &deckCards)
{
    if (!deckName.isEmpty() && !mDeckNameMap.contains(deckName) && !deckCards.isEmpty())
    {
        CDeck deck(deckName, type);
        for (int iId = 0; iId < deckCards.size(); ++iId)
        {
            QHash<unsigned int, CCard*>::const_iterator iCard = mCardIdMap.find(deckCards.at(iId));
            if (iCard != mCardIdMap.end() && iCard.key() == deckCards.at(iId))
            {
                deck.addCard(*iCard.value());
            }
            else
            {
                return;
            }
        }
        if (deck.isValid())
        {
            mDeckNameMap.insert(deckName, deck);
            if (type == ERaidDeckType)
            {
                mRaidDecks.push_back(deckName);
            }
        }
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
    mDeckNameMap.clear();
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
    mCustomDecks.clear();
    mRaidDecks.clear();
    mBattlegrounds.clear();
    mAchievements.clear();
    mTotalDownloads = 0;
    mFinishedDownloads = 0;

    const CPathManager& pm = CPathManager::getPathManager();

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

    //load missions
    QFile missionFile(pm.getToolPath() + "missions.xml");
    if (missionFile.open(QIODevice::ReadOnly))
    {
        CMissionsXmlParser missionXmlParser;
        connect(
            &missionXmlParser, SIGNAL(missionParsed(const QString&, EDeckType, const QList<unsigned int>&)),
            this, SLOT(processDeck(const QString&, EDeckType, const QList<unsigned int>&)));

        QXmlInputSource missionXml(&missionFile);
        QXmlSimpleReader missionReader;
        missionReader.setContentHandler(&missionXmlParser);
        missionReader.parse(missionXml);
        missionFile.close();
    }

    //load raids
    QFile raidFile(pm.getToolPath() + "raids.xml");
    if (raidFile.open(QIODevice::ReadOnly))
    {
        CRaidsXmlParser raidXmlParser;
        connect(
            &raidXmlParser, SIGNAL(raidParsed(const QString&, EDeckType, const QList<unsigned int>&)),
            this, SLOT(processDeck(const QString&, EDeckType, const QList<unsigned int>&)));

        QXmlInputSource raidXml(&raidFile);
        QXmlSimpleReader raidReader;
        raidReader.setContentHandler(&raidXmlParser);
        raidReader.parse(raidXml);
        raidFile.close();
    }

    //load custom decks
    QFile customDeckFile(pm.getToolPath() + "custom.txt");
    if (customDeckFile.open(QIODevice::ReadOnly))
    {
        while (!customDeckFile.atEnd())
        {
            QString customDeckStr(customDeckFile.readLine());
            QStringList customDeckTokens = customDeckStr.split(QRegExp("\\:"), QString::SkipEmptyParts);
            if (customDeckTokens.size() > 1)
            {
                CDeck customDeck;
                QString customDeckName = customDeckTokens.at(0).trimmed();
                if (!customDeckName.isEmpty() && !mDeckNameMap.contains(customDeckName) && strToDeck(customDeckTokens.at(1), customDeck))
                {
                    customDeck.setName(customDeckName);
                    customDeck.setType(ECustomDeckType);
                    mDeckNameMap.insert(customDeckName, customDeck);
                    mCustomDecks.push_back(customDeckName);
                }
            }
        }
    }

    //load quests
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
