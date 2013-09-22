#ifndef CARDTABLE_H
#define CARDTABLE_H

#include <QHash>
#include <QMultiHash>
#include <QQueue>
#include <QStringList>
#include <QNetworkAccessManager>
#include "model/CSkill.h"
#include "model/CBattleground.h"
#include "model/CAchievement.h"
#include "ICardCheck.h"

class CDownload;
class CPictureDownload;

struct SCardStatus
{
    SCardStatus() : numOwned(-1), numOwnedFiltered(-1), isBlack(false), isWhite(false) { }
    int numOwned;
    int numOwnedFiltered;
    bool isBlack;
    bool isWhite;
};

typedef QMap<unsigned int, SCardStatus> TCardStatusMap;
typedef QPair<CCard, int> TOwnedCard;

enum ECardStatusUpdate
{
    ENoStatusUpdate,
    EOwnedStatusUpdate,
    EListStatusUpdate
};

class CCardTable : public QObject
{
    Q_OBJECT

public:
    CCardTable();
    virtual ~CCardTable();
    static CCardTable& getCardTable();

public:
    void refresh();

    const CSkill& getSkillForId(const QString id) const;
    const QString getPictureForCardSet(ECardSet cardSet) const;
    const CCard& getCardForId(unsigned int id) const;
    const CCard& getCardForName(const QString &name) const;
    void getCardsForName(const QString &name, QList<CCard*> &cards) const;
    void searchCards(const ICardCheck &search, QList<CCard*> &cards, int maxHits = -1) const;

    const CCard& getOwnedCardEquivalent(const CCard &card, const TCardStatusMap &used) const;
    SCardStatus getCardStatus(const CCard &card) const;
    bool isCardOwned(const CCard &card) const;
    void setListedCards(const QStringList &blackList, const QStringList &whiteList);
    void setOwnedCards(const QList<TOwnedCard> &ownedCards, const QList<TOwnedCard> &filteredCards);

    void updateData(bool beta);
    const CBattleground& getBattlegroundForId(unsigned int id) const;
    const QList<CBattleground>& getBattlegrounds() const;
    const QList<CAchievement>& getAchievements() const;

signals:
    void downloadProgress(int numDone, int numDownloads);
    void dataUpdated(const QStringList &result);
    void cardStatusUpdated(ECardStatusUpdate status);

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
    QMultiHash<QString, CCard*> mCardNameMap;
    QHash<unsigned int, SCardStatus> mCardStatusMap;
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
