#ifndef CDECKSAVEWIDGET_H
#define CDECKSAVEWIDGET_H

#include <QWidget>
#include "model/CDeck.h"
#include "process/CProcessParameters.h"

namespace Ui {
class DeckSaveWidget;
}

enum EDeckSaveResult
{
    EDeckSaveOk,
    EDeckSaveCancel
};

class CDeckSaveWidget : public QWidget
{
    Q_OBJECT
    
public:
    CDeckSaveWidget(const CDeck &deck, EOptimizationMode optMode, QWidget *parent = 0);
    ~CDeckSaveWidget();

public:
    EDeckSaveResult result() const { return mResult; }
    QString deckName() const;

public:
    static QString buildDeckName(const CDeck &deck, EOptimizationMode optMode);
    static QString buildCardName(const CCard &card);

protected slots:
    void okButtonPressed();
    void cancelButtonPressed();
    
private:
    Ui::DeckSaveWidget *mUi;
    EDeckSaveResult mResult;
};

#endif // CDECKSAVEWIDGET_H
