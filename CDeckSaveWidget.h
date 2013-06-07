#ifndef CDECKSAVEWIDGET_H
#define CDECKSAVEWIDGET_H

#include <QWidget>

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
    CDeckSaveWidget(const QString &deckName, QWidget *parent = 0);
    ~CDeckSaveWidget();

public:
    EDeckSaveResult result() const { return mResult; }
    QString deckName() const;

protected slots:
    void okButtonPressed();
    void cancelButtonPressed();
    
private:
    Ui::DeckSaveWidget *mUi;
    EDeckSaveResult mResult;
};

#endif // CDECKSAVEWIDGET_H
