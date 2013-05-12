#ifndef CDECKMANAGEMENTWIDGET_H
#define CDECKMANAGEMENTWIDGET_H

#include <QWidget>
#include <QItemSelection>
#include <QStyledItemDelegate>
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

signals:
    void setDeck(const QString &deckStr, EInputDeckTarget target);

public slots:
    void updateButtonAvailability();

private slots:
    void deleteSelectedDeck();
    void setSelectedBaseDeck();
    void setSelectedEnemyDeck();
    void setSelectedDeck();
    
private:
    Ui::DeckManagementWidget *mUi;
    CDeckIconDelegate mDeckIconDelegate;
    CDeckTable &mDecks;
};

#endif // CDECKMANAGEMENTWIDGET_H
