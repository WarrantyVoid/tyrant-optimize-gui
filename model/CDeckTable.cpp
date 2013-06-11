#include "CDeckTable.h"
#include "CGlobalConfig.h"
#include "CCardTable.h"
#include "xml/CMissionsXmlParser.h"
#include "xml/CRaidsXmlParser.h"
#include "xml/CQuestsXmlParser.h"
#include <QPixmap>
#include <QFile>
#include <QMimeData>

CDeckTable* CDeckTable::DECK_TABLE = 0;
const QString CDeckTable::BASE_64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const QChar CDeckTable::BASE_64_CHAR_EXT = '-';

CDeckTable::CDeckTable(QObject *parent)
: QAbstractTableModel(parent)
, mDecks()
, mDeckNameMap()
{
    initData();
}

CDeckTable::~CDeckTable()
{
    clearDecks();
}

CDeckTable& CDeckTable::getDeckTable()
{
    if (!DECK_TABLE)
    {
        DECK_TABLE = new CDeckTable();
    }
    return *DECK_TABLE;
}

bool CDeckTable::deckToHash(const CDeck &deck, QString &hashStr) const
{
    hashStr.clear();
    const QList<CCard> &cards = deck.getCards();
    unsigned int lastId(0);
    unsigned int num(0);
    for (QList<CCard>::const_iterator iCard = cards.begin(); iCard != cards.end(); ++iCard)
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

bool CDeckTable::hashToDeck(const QString &hashStr, CDeck &deck) const
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
    CCardTable &cards = CCardTable::getCardTable();
    for (QList<unsigned int>::const_iterator i = ids.begin(); i != ids.end(); ++i)
    {
        const CCard &curCard = cards.getCardForId(*i);
        if (curCard.isValid())
        {
            deck.addCard(curCard);
        }
        else
        {
            return false;
        }
    }
    return deck.isValid();
}

bool CDeckTable::strToDeck(const QString &deckStr, CDeck &deck) const
{
    QStringList customDeckTokens = deckStr.split(QRegExp("\\,"), QString::SkipEmptyParts);
    QStringList customDeckCards;
    deck.setName(deckStr);
    deck.clearCards();
    CCardTable &cards = CCardTable::getCardTable();
    if (customDeckTokens.size() > 0)
    {
        for (int iToken = 0;  iToken < customDeckTokens.size(); ++iToken)
        {
            int num = 1;
            QString curCardStr = customDeckTokens.at(iToken).trimmed();
            CCard curCard = cards.getCardForName(curCardStr);

            if (iToken > 0 && !curCard.isValid())
            {
                QStringList cardSplitNum = curCardStr.split(QRegExp("\\#"), QString::SkipEmptyParts);
                if (cardSplitNum.size() > 1)
                {
                    curCardStr = cardSplitNum.at(0).trimmed();
                    curCard = cards.getCardForName(curCardStr);
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
                    curCard = cards.getCardForId(cardId);
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

const CDeck& CDeckTable::getDeckForIndex(const QModelIndex &index) const
{
    if (index.isValid())
    {
        return *mDecks[index.row()];
    }
    return CDeck::INVALID_DECK;
}

const CDeck& CDeckTable::getDeckForName(const QString &deckName) const
{
    QHash<QString,CDeck*>::const_iterator i =  mDeckNameMap.find(deckName);
    if (i != mDeckNameMap.end() && i.key() == deckName)
    {
        return *i.value();
    }
    return CDeck::INVALID_DECK;
}

bool CDeckTable::addCustomDeck(CDeck &customDeck)
{
    bool deckCreated = false;
    QHash<QString, CDeck*>::iterator iDeck = mDeckNameMap.find(customDeck.getName());
    if (iDeck != mDeckNameMap.end() && iDeck.key() == customDeck.getName())
    {
        *iDeck.value() = customDeck;
    }
    else
    {
        CDeck *newDeck = new CDeck(customDeck);
        int row = 0;
        for (; row < mDecks.size(); ++row)
        {
            if (mDecks[row]->getType() != ECustomDeckType || mDecks[row]->getName().compare(customDeck.getName()) > 0)
            {
                break;
            }
        }
        beginInsertRows(QModelIndex(), row, row);
        mDecks.insert(row, newDeck);
        mDeckNameMap.insert(customDeck.getName(), newDeck);
        deckCreated = true;
        endInsertRows();
    }
    writeCustomDecksFile();
    return deckCreated;
}

bool CDeckTable::deleteCustomDecks(const QStringList &customDecks)
{
    bool deckDeleted = false;
    for(QStringList::const_iterator i = customDecks.begin(); i != customDecks.end(); ++i)
    {
        QHash<QString, CDeck*>::iterator iDeck = mDeckNameMap.find(*i);
        if (iDeck != mDeckNameMap.end() && iDeck.key() == *i)
        {
            int row = mDecks.indexOf(*iDeck);
            if (row > -1)
            {
                beginRemoveRows(QModelIndex(), row, row);
                mDecks.removeAt(row);
                mDeckNameMap.remove(*i);
                deckDeleted = true;
                endRemoveRows();
            }
        }
    }
    writeCustomDecksFile();
    return deckDeleted;
}

void CDeckTable::getCustomDecks(QStringList &customDecks) const
{
    for (QList<CDeck*>::const_iterator i = mDecks.begin(); i != mDecks.end(); ++i)
    {
        if ((*i)->getType() == ECustomDeckType)
        {
            customDecks.push_back((*i)->getName());
        }
        else
        {
            break;
        }
    }
}

void CDeckTable::clearDecks()
{
    for (QList<CDeck*>::iterator i = mDecks.begin(); i != mDecks.end(); ++i)
    {
        delete *i;
        *i = 0;
    }
    beginResetModel();
    mDecks.clear();
    mDeckNameMap.clear();
    endResetModel();
}

int CDeckTable::rowCount(const QModelIndex &/*parent*/) const
{
    return mDecks.size();
}

int CDeckTable::columnCount(const QModelIndex &/*parent*/) const
{
    return 3;
}

QVariant CDeckTable::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        CDeck *deckData = mDecks[index.row()];
        switch (index.column())
        {
        case 0:
            switch (role)
            {
            case Qt::DisplayRole:
                switch(deckData->getType())
                {
                case EMissionDeckType: return QChar('M');
                case ERaidDeckType: return QChar('R');
                case EQuestDeckType: return QChar('Q');
                case ECustomDeckType: return QChar('C');
                default: return 'U';
                }
            case Qt::TextAlignmentRole:
                return Qt::AlignCenter;
            case Qt::ToolTipRole:
                return QVariant();
            }
            break;
        case 1:
        {
            switch (role)
            {
            case Qt::DecorationRole:
            {
                QPixmap comImg;
                comImg.load(CGlobalConfig::getCfg().getPicturePath() + mDecks[index.row()]->getCommander().getPicture());
                return QVariant(comImg.scaledToHeight(20, Qt::SmoothTransformation));
            }
            case Qt::ToolTipRole:
                return QVariant();
            }
            break;
        }
        case 2:
            switch (role)
            {
            case Qt::DisplayRole:
                return QVariant(mDecks[index.row()]->getName());
            case Qt::ToolTipRole:
                return QVariant();
            }
            break;
        }
    }
    return QVariant();
}

QVariant CDeckTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            if (role == Qt::DecorationRole)
            {
                QPixmap comImg;
                comImg.load(":/img/tyrant/CardBack.png");

                return QVariant(comImg.scaledToHeight(20));
            }
            break;
        case 1:
        {
            if (role == Qt::DecorationRole)
            {
                QPixmap comImg;
                comImg.load(":/img/tyrant/CommanderRegIcon.png");
                return QVariant(comImg.scaledToHeight(20));
            }
            break;
        }
        case 2:
            if (role == Qt::DisplayRole)
            {
                return "Deck Name";
            }
            break;
        }
    }
    return QVariant();
}

Qt::DropActions CDeckTable::supportedDropActions() const
 {
     return Qt::MoveAction;
 }

Qt::ItemFlags CDeckTable::flags( const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (index.isValid())
    {
        flags |= Qt::ItemIsDragEnabled;
        //CDeck *deckData = mDecks[index.row()];
        //if (deckData->getType() == ECustomDeckType)
        //{
        //    flags |= Qt::ItemIsEditable;
        //}
    }
    return flags;
}

QStringList CDeckTable::mimeTypes() const
{
    QStringList types;
    types << "application/vnd.text.list";
    return types;
}

QMimeData *CDeckTable::mimeData(const QModelIndexList &indexes) const
 {
     QMimeData *mimeData = new QMimeData();
     QStringList deckStr;
     foreach (QModelIndex index, indexes)
     {
         if (index.isValid() && index.column() == 2)
         {
             deckStr << data(index, Qt::DisplayRole).toString();
         }
     }
     mimeData->setText(deckStr.join(";"));
     return mimeData;
 }

void CDeckTable::processDeck(const QString &deckName, EDeckType type, unsigned int battlegroundId, const QList<unsigned int> &deckCards)
{
    if (!deckName.isEmpty() && !deckCards.isEmpty())
    {
        CCardTable &cards = CCardTable::getCardTable();
        CDeck* deck = new CDeck(deckName, type, battlegroundId);
        for (QList<unsigned int>::const_iterator i = deckCards.begin(); i != deckCards.end(); ++i)
        {
            const CCard &curCard = cards.getCardForId(*i);
            deck->addCard(curCard);
        }
        if (deck->isValid() && !mDeckNameMap.contains(deck->getName()))
        {
            mDecks.push_back(deck);
            mDeckNameMap.insert(deck->getName(), deck);
        }
        else
        {
            delete deck;
            deck = 0;
        }
    }
}

void CDeckTable::initData()
{
    const CGlobalConfig& pm = CGlobalConfig::getCfg();
    clearDecks();

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
                CDeck *customDeck = new CDeck();
                QString customDeckName = customDeckTokens.at(0).trimmed();
                if (!customDeckName.isEmpty() && !mDeckNameMap.contains(customDeck->getName()) && strToDeck(customDeckTokens.at(1), *customDeck))
                {
                    customDeck->setName(customDeckName);
                    customDeck->setType(ECustomDeckType);
                    mDecks.push_back(customDeck);
                    mDeckNameMap.insert(customDeck->getName(), customDeck);
                }
                else
                {
                    delete customDeck;
                    customDeck = 0;
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
            &questXmlParser, SIGNAL(questParsed(const QString&, EDeckType, unsigned int, const QList<unsigned int>&)),
            this, SLOT(processDeck(const QString&, EDeckType, unsigned int, const QList<unsigned int>&)));

        QXmlInputSource questXml(&questFile);
        QXmlSimpleReader questReader;
        questReader.setContentHandler(&questXmlParser);
        questReader.parse(questXml);
        questFile.close();
    }

    //load raids
    QFile raidFile(pm.getToolPath() + "raids.xml");
    if (raidFile.open(QIODevice::ReadOnly))
    {
        CRaidsXmlParser raidXmlParser;
        connect(
            &raidXmlParser, SIGNAL(raidParsed(const QString&, EDeckType, unsigned int, const QList<unsigned int>&)),
            this, SLOT(processDeck(const QString&, EDeckType, unsigned int, const QList<unsigned int>&)));

        QXmlInputSource raidXml(&raidFile);
        QXmlSimpleReader raidReader;
        raidReader.setContentHandler(&raidXmlParser);
        raidReader.parse(raidXml);
        raidFile.close();
    }

    //load missions
    QFile missionFile(pm.getToolPath() + "missions.xml");
    if (missionFile.open(QIODevice::ReadOnly))
    {
        CMissionsXmlParser missionXmlParser;
        connect(
            &missionXmlParser, SIGNAL(missionParsed(const QString&, EDeckType, unsigned int, const QList<unsigned int>&)),
            this, SLOT(processDeck(const QString&, EDeckType, unsigned int, const QList<unsigned int>&)));

        QXmlInputSource missionXml(&missionFile);
        QXmlSimpleReader missionReader;
        missionReader.setContentHandler(&missionXmlParser);
        missionReader.parse(missionXml);
        missionFile.close();
    }
}

bool CDeckTable::writeCustomDecksFile()
{
    QFile customDeckFile(CGlobalConfig::getCfg().getToolPath() + "custom.txt");
    if (customDeckFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        QStringList customDecks;
        getCustomDecks(customDecks);
        customDecks.sort();
        QTextStream out(&customDeckFile);
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
            const CDeck &deck = getDeckForName(*iDeck);
            if (deck.isValid())
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
        return true;
    }
    return false;
}
