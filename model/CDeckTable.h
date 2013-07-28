#ifndef CDECKTABLE_H
#define CDECKTABLE_H

#include <QAbstractTableModel>
#include <QList>
#include <QHash>
#include <QSet>
#include "model/CDeck.h"

class CDeckTable : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit CDeckTable(QObject *parent = 0);
    virtual ~CDeckTable();
    static CDeckTable& getDeckTable();

public:
    void refresh();

    bool deckToHash(const CDeck &deck, QString &hashStr) const;
    bool hashToDeck(const QString &hashStr, CDeck &deck) const;
    bool deckToStr(const CDeck &deck, QString &deckStr, bool forceId = false) const;
    bool strToDeck(const QString &deckStr, CDeck &deck) const;
    const CDeck& getDeckForIndex(const QModelIndex &index) const;
    const CDeck& getDeckForName(const QString &deckName) const;


    bool addCustomDeck(CDeck &customDeck);
    bool deleteCustomDecks(const QStringList &customDecks);
    void getCustomDecks(QStringList &customDecks) const;
    void clearDecks();

    bool isDeckBlocked(const QString &deckStr) const;
    void setDeckBlockage(const QString &deckStr, bool isBlocked);

protected slots:
    void processDeck(unsigned int id, const QString &deckName, EDeckType type, unsigned int battlegroundId, const QList<unsigned int> &deckCards);

protected:
    void initData();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual Qt::DropActions supportedDropActions() const;
    virtual Qt::ItemFlags flags( const QModelIndex & index) const;
    virtual QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;

private:
    bool writeCustomDecksFile();

private:
    QList<CDeck*> mDecks;
    QHash<QString, CDeck*> mDeckNameMap;
    QSet<QString> mBlockedDecks;
    static CDeckTable *DECK_TABLE;
    static const QString BASE_64_CHARS;
    static const QChar BASE_64_CHAR_EXT;
};

#endif // CDECKTABLE_H
