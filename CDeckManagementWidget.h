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
* Painting delegate:
* Hides focus box and places icons correctly in table cells
*/
class CDeckItemDelegate : public QStyledItemDelegate
{
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItem opt = option;
        opt.decorationPosition = QStyleOptionViewItem::Right;
        opt.state &= ~QStyle::State_HasFocus;
        if (index.column() == EDeckCommanderColumn)
        {
            QRect normRect = option.rect;
            normRect.moveTo(0, 0);
            painter->drawPixmap(option.rect, index.data(Qt::DecorationRole).value<QPixmap>(), normRect);

        }
        else
        {
            QStyledItemDelegate::paint(painter, opt, index);
        }
    }

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItem opt = option;
        opt.decorationPosition = QStyleOptionViewItem::Right;
        opt.state &= ~QStyle::State_HasFocus;
        QStyledItemDelegate::updateEditorGeometry(editor, opt, index);
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
        case EDeckIdColumn: break;
        case EDeckTypeColumn: mTypePattern = QRegExp(pattern); break;
        case EDeckCommanderColumn: mCommanderPattern = pattern.toLower(); break;
        case EDeckSizeColumn: break;
        case EDeckNameColumn: mNamePattern = pattern.toLower(); break;
        }
    }

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
    {
        if (!mTypePattern.isEmpty())
        {
            QModelIndex index = sourceModel()->index(sourceRow, EDeckTypeColumn, sourceParent);
            if (!mTypePattern.exactMatch(index.data().toString()))
            {
                return false;
            }
        }
        if (!mCommanderPattern.isEmpty())
        {
            QModelIndex index = sourceModel()->index(sourceRow, EDeckCommanderColumn, sourceParent);
            if (!index.data().toString().toLower().contains(mCommanderPattern))
            {
                return false;
            }
        }
        if (!mNamePattern.isEmpty())
        {
            QModelIndex index = sourceModel()->index(sourceRow, EDeckNameColumn, sourceParent);
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

public slots:
    bool addCustomDeck(CDeck &customDeck);
    void updateButtonAvailability();
    void updateView();

private slots:
    void deleteSelectedDeck();
    void blockSelectedDeck();
    void setSelectedBaseDeck();
    void setSelectedEnemyDeck();
    void setSelectedDeck();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *e);

private:
    void toggleDeckUsageButton(bool curDeckUsed);
    
private:
    Ui::DeckManagementWidget *mUi;
    CDeckTable &mDecks;
    CDeckItemDelegate mDeckItemDelegate;
    DeckSortFilterProxyModel mDeckSortProxy;
};

#endif // CDECKMANAGEMENTWIDGET_H
