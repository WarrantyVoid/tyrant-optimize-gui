#ifndef CMULTIDECKWIDGET_H
#define CMULTIDECKWIDGET_H

#include <QWidget>
#include <QPushButton>
#include "CDeckInput.h"

class CMultiDeckEditorWidget;

class CMultiDeckWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CMultiDeckWidget(QWidget *parent = 0);

public:
    void setDeckInputWidget(CDeckInput* inputWidget);    
    void updateHistory();    
    void initDecks();

signals:
    void numberOfDecksChanged(int numDecks);
    void decksUpdated(const QString &decks);
    void deckToolTipTriggered(bool isVisible, const QString &deckId = "");

public slots:
    void declineDecks();
    void acceptDecks();

protected slots:
    void addDeck();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual bool eventFilter(QObject *obj, QEvent *e);

private:
    bool isDoubleEqual(double d1, double d2);
    void resetEditors();

private:
    static const int MAX_NUMBER_OF_DECKS = 30;

    CDeckInput* mDeckSourceWidget;
    CMultiDeckEditorWidget* mMultiDeckEditors[MAX_NUMBER_OF_DECKS];
    QPushButton *mAddButton;
    QPushButton *mCancelButton;
    QPushButton *mOkButton;
    int mNumberOfDecks;

};

#endif // CMULTIDECKWIDGET_H
