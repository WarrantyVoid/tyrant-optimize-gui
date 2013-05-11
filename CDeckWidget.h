#ifndef CDECKWIDGET_H
#define CDECKWIDGET_H

#include <QWidget>
#include "model/CDeckTable.h"

namespace Ui
{
class DeckWidget;
}

#include <QDrag>
#include <QMimeData>
#include "CCardLabel.h"

class CDeckWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CDeckWidget(QWidget *parent = 0);

public:
    void setDropEnabled(bool enabled);
    void setWinLabel(const QString &text);
    void setWinLabel(const QPixmap &pixmap);
    void setDefaultUnits();
    void setUnit(int slot, const CCard &unit);
    void setDeck(const CDeck &deck);
    const CCard& getUnit(int slot) const;
    const CDeck& getDeck() const;

signals:
    void deckChanged(const QString& deckId);

public slots:
    void setDeck(const QString & deckId);

protected slots:
    void updateDeck();

private:
    Ui::DeckWidget *mUi;
    CDeckTable &mDecks;

    // @todo make this member useful
    mutable CDeck mDeck;
};

#endif // CDECKWIDGET_H
