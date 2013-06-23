#ifndef CDECKMANAGEMENTWIDGET_H
#define CDECKMANAGEMENTWIDGET_H

#include <QWidget>
#include <QItemSelection>
#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include <QSettings>
#include <QPainter>
#include "model/CDeckTable.h"

namespace Ui
{
class DeckManagementWidget;
}

/**
* Workaround for avoiding annyoying left margin when paiting icons in table cells.
*/
class CDeckIconDelegate : public QStyledItemDelegate
{
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QRect normRect = option.rect;
        normRect.moveTo(0, 0);
        painter->drawPixmap(option.rect, index.data(Qt::DecorationRole).value<QPixmap>(), normRect);
    }
};

/**
* Workaround for allowing multiple column filtering
*/
class DeckSortFilterProxyModel : public QSortFilterProxyModel
{
Q_OBJECT
public:
    void setFilterPattern(int column, const QString &pattern)
    {
        switch(column)
        {
        case 0: mTypePattern = QRegExp(pattern); break;
        case 1: mCommanderPattern = pattern.toLower(); break;
        case 3: mNamePattern = pattern.toLower(); break;
        default:  break;
        }
    }

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
    {
        if (!mTypePattern.isEmpty())
        {
            QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
            if (!mTypePattern.exactMatch(index.data().toString()))
            {
                return false;
            }
        }
        if (!mCommanderPattern.isEmpty())
        {
            QModelIndex index = sourceModel()->index(sourceRow, 1, sourceParent);
            if (!index.data().toString().toLower().contains(mCommanderPattern))
            {
                return false;
            }
        }
        if (!mNamePattern.isEmpty())
        {
            QModelIndex index = sourceModel()->index(sourceRow, 3, sourceParent);
            if (!index.data().toString().toLower().contains(mNamePattern))
            {
                return false;
            }
        }
        return true;
    }

private:
    QRegExp mTypePattern;
    QString mCommanderPattern;
    QString mNamePattern;
};

enum EInputDeckTarget
{
    BaseDeckInputTarget,
    EnemyDeckInputTarget,
    ActiveDeckInputTarget
};

class CDeckManagementWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CDeckManagementWidget(QWidget *parent = 0);
    ~CDeckManagementWidget();

public:
    void loadParameterSettings(QSettings &settings);
    void saveParameterSettings(QSettings &settings);

signals:
    void setDeck(const QString &deckStr, EInputDeckTarget target);
    void blackListCards(const QStringList &cards, bool toBlack);

public slots:
    bool addCustomDeck(CDeck &customDeck);
    void updateButtonAvailability();
    void updateView();

private slots:
    void deleteSelectedDeck();
    void blacklistSelectedDeck();
    void setSelectedBaseDeck();
    void setSelectedEnemyDeck();
    void setSelectedDeck();

private:
    void addDeckToBlackList(const CDeck& deck, QStringList &blackList);
    
private:
    Ui::DeckManagementWidget *mUi;
    CDeckTable &mDecks;
    CDeckIconDelegate mDeckIconDelegate;
    DeckSortFilterProxyModel mDeckSortProxy;
};

#endif // CDECKMANAGEMENTWIDGET_H
