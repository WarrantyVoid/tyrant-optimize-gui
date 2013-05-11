#ifndef CDECKINPUT_H
#define CDECKINPUT_H

#include <QComboBox>
#include <QDropEvent>

class CDeckInput : public QComboBox
{
    Q_OBJECT

public:
    CDeckInput(QWidget *parent = 0);

public:
    void setAllowMultiDecks(bool allow);
    QStringList history() const;

public slots:
    void setDeckId(const QString &deckStr);
    void updateHistory(const QStringList &history);
    void updateHistory();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *e);

private:
    bool mAreMultiDecksAllowed;
};

#endif // CDECKINPUT_H
