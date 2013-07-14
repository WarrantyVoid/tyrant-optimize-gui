#ifndef CARDTABLE_H
#define CARDTABLE_H

#include <QHash>
#include <QQueue>
#include <QStringList>
#include <QNetworkAccessManager>
#include "model/CSkill.h"
#include "model/CBattleground.h"
#include "model/CAchievement.h"
#include "ICardCheck.h"

class CDownload;
class CPictureDownload;
typedef QPair<CCard, int> TOwnedCard;

struct SCardStatus
{
    SCardStatus(int o, int f) : numOwned(o), numOwnedFiltered(f) { }
    SCardStatus() : numOwned(0), numOwnedFiltered(0) { }
    int numOwned;
    int numOwnedFiltered;
};

class CCardTable : public QObject
{
    Q_OBJECT

public:
    CCardTable();
    virtual ~CCardTable();
    static CCardTable& getCardTable();

public:
    const CSkill& getSkillForId(const QString id) const;
    const QString getPictureForCardSet(ECardSet cardSet) const;
    const CCard& getCardForId(unsigned int id) const;
    const CCard& getCardForName(const QString &name) const;
    void searchCards(const ICardCheck &search, QList<CCard*> &cards, int maxHits = -1) const;

    SCardStatus getCardStatus(const CCard *card) const;
    bool isCardOwned(const CCard *card) const;

    void setOwnedCards(const QList<TOwnedCard> &ownedCards, const QList<TOwnedCard> &filteredCards);
    void updateData();
    const CBattleground& getBattlegroundForId(unsigned int id) const;
    const QList<CBattleground>& getBattlegrounds() const;
    const QList<CAchievement>& getAchievements() const;

signals:
    void downloadProgress(int numDone, int numDownloads);
    void dataUpdated(const QStringList &result);
    void ownedCardsUpdated();

 protected slots:
    void processNextPictureDownload();
    void processNextDataDownload();
    void processSkill(const CSkill& skill);
    void processCard(const CCard &card);
    void processCardSet(ECardSet cardSet, const QString &picture);
    void processBattleground(const CBattleground& battleground);
    void processAchievement(const CAchievement& achievement);

protected:
    void initData();

private:
    QHash<QString, CSkill> mSkillIdMap;
    QHash<ECardSet, QString> mCardSetIdMap;
    QHash<unsigned int, CCard*> mCardIdMap;
    QHash<QString, CCard*> mCardNameMap;
    QHash<unsigned int, SCardStatus> mOwnedCardMap;
    QNetworkAccessManager* mNetManager;
    QQueue<CPictureDownload*> mPictureDownloads;
    QQueue<CDownload*> mDataDownloads;
    QStringList mDataDownloadResults;
    QList<CBattleground> mBattlegrounds;
    QList<CAchievement> mAchievements;
    int mTotalDownloads;
    int mFinishedDownloads;    
    
    static CCardTable *CARD_TABLE;
};

#endif // CARDTABLE_H
