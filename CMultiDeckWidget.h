#ifndef CMULTIDECKWIDGET_H
#define CMULTIDECKWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>

class CMultiDeckEditorWidget;

class CMultiDeckWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CMultiDeckWidget(QWidget *parent = 0);

public:
    void setDeckWidget(QLineEdit* editWidget);
    void updateAvailableDecks();
    void resetEditors();

signals:
    void numberOfDecksChanged(int numDecks);
    void decksUpdated(const QString &decks);

public slots:
    void declineDecks();
    void acceptDecks();

protected slots:
    void addDeck();

protected:
    virtual void showEvent(QShowEvent *event);

private:
    bool isDoubleEqual(double d1, double d2);

private:
    QLineEdit* mDeckSourceWidget;
    CMultiDeckEditorWidget* mMultiDeckEditors[10];
    QPushButton *mAddButton;
    QPushButton *mCancelButton;
    QPushButton *mOkButton;
    int mNumberOfDecks;
};

#endif // CMULTIDECKWIDGET_H
