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
#include "process/IProcessWrapper.h"
#include "process/CProcessParameters.h"

enum EDeckSpecificationType
{
    ENoSpecification,
    ENameSpecification,
    EHashSpecification,
    ECardListSpecification
};

class CDeckWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CDeckWidget(QWidget *parent = 0);

public:
    void setLockEnabled(bool enabled);
    void setLocked(int slot, bool locked);
    bool isLocked(int slot) const;

    void setDropEnabled(bool enabled);
    void setWinLabel(const SOptimizationStatus &status, EOptimizationMode mode);
    void setWinLabel(const QPixmap &pixmap);
    void setDefaultUnits();
    void setDeck(const CDeck &deck);
    const CDeck& getDeck() const;

signals:
    void deckChanged(const QString& deckId);

public slots:
    void setDeck(const QString & deckSpec, EDeckSpecificationType specType = ENoSpecification);
    void updateView();

protected slots:
    void updateDeck();

private:
    const CCard& getUnit(int slot) const;
    void setUnit(int slot, const CCard &unit);
    void syncAllUnits();

private:
    CCardLabel* getLabelForSlot(int slot) const;

private:
    Ui::DeckWidget *mUi;
    QList<CCardLabel*> mCardLabels;
    CDeckTable &mDecks;
    bool mIsLocked;
    CDeck mDeck;
};

#endif // CDECKWIDGET_H
