#ifndef CARDTABLE_H
#define CARDTABLE_H

#include <QHash>
#include <QQueue>
#include <QStringList>
#include <QNetworkAccessManager>
#include "model/CDeck.h"
#include "model/CSkill.h"
#include "model/CBattleground.h"

class CDownload;
class CPictureDownload;
typedef QPair<QString, QString> TAchievement;

class CCardTable : public QObject
{
    Q_OBJECT

public:
    CCardTable();
    virtual ~CCardTable();
    static CCardTable& getCardTable();

public:
    bool deckToHash(const CDeck &deck, QString &hashStr) const;
    bool hashToDeck(const QString &hashStr, CDeck &deck) const;
    bool nameToDeck(const QString &deckName, CDeck &deck) const;
    bool strToDeck(const QString &deckStr, CDeck &deck) const;
    const CSkill& getSkillForId(const QString id) const;
    const QString getPictureForCardSet(ECardSet cardSet) const;
    const CCard& getCardForId(unsigned int id) const;
    const CCard& getCardForName(const QString &name) const;
    void searchCards(const QString &search, QList<CCard*> &cards, int maxHits = -1) const;

    void updateData();
    bool addCustomDeck(CDeck &customDeck);
    const QStringList& getCustomDecks();
    const QStringList& getRaidDecks();
    const QList<CBattleground>& getBattlegrounds();
    const QList<TAchievement>& getAchievements();

signals:
    void downloadProgress(int numDone, int numDownloads);
    void dataUpdated(const QStringList &result);

 protected slots:
    void processNextPictureDownload();
    void processNextDataDownload();
    void processSkill(const CSkill& skill);
    void processCard(const CCard &card);
    void processCardSet(ECardSet cardSet, const QString &picture);
    void processDeck(const QString &deckName, EDeckType type, const QList<unsigned int> &deckCards);
    void processBattleground(const CBattleground& battleground);
    void processAchievement(const QString &name, const QString& description);

protected:
    void initData();

private:
    QHash<QString, CSkill> mSkillIdMap;
    QHash<ECardSet, QString> mCardSetIdMap;
    QHash<unsigned int, CCard*> mCardIdMap;
    QHash<QString, CCard*> mCardNameMap;
    QHash<QString, CDeck> mDeckNameMap;
    QNetworkAccessManager* mNetManager;
    QQueue<CPictureDownload*> mPictureDownloads;
    QQueue<CDownload*> mDataDownloads;
    QStringList mDataDownloadResults;
    QStringList mCustomDecks;
    QStringList mRaidDecks;
    QList<CBattleground> mBattlegrounds;
    QList<TAchievement> mAchievements;
    int mTotalDownloads;
    int mFinishedDownloads;    
    
    static CCardTable *CARD_TABLE;
    static const QString BASE_64_CHARS;
    static const QChar BASE_64_CHAR_EXT;
};

#endif // CARDTABLE_H
