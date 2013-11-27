#ifndef CCARDLABEL_H
#define CCARDLABEL_H

#include <QLabel>
#include <QPushButton>
#include "model/CCardTable.h"

class QMimeData;

class CCardLabel : public QLabel
{
    Q_OBJECT

public:
    explicit CCardLabel(QWidget *parent = 0);
    virtual ~CCardLabel();

public:
    void setLockEnabled(bool enabled);
    void setLocked(bool locked);
    bool isLocked() const;

    void setCard(const CCard& card);
    const CCard& getCard() const;

    static QMimeData *createCardLabelDropData(const CCardLabel &label);
    static bool isCardLabelDropData(const QMimeData *data);

signals:
    void unitDragged();
    void unitDropped();
    void unitDoubleClicked();
    void unitRightClicked(bool isBlack);
    void unitCtrlRightClicked(bool isWhite);    

protected slots:
    void actionToggleBlack(bool isBlack);
    void actionToggleWhite(bool isWhite);

protected:
    virtual void paintEvent(QPaintEvent *ev);
    virtual void mousePressEvent(QMouseEvent * ev);
    virtual void mouseReleaseEvent(QMouseEvent * ev);
    virtual void mouseDoubleClickEvent(QMouseEvent * ev);
    virtual void mouseMoveEvent(QMouseEvent * ev);
    virtual void leaveEvent(QEvent *ev);
    virtual void dropEvent(QDropEvent *ev);
    virtual void dragMoveEvent(QDragMoveEvent *ev);
    virtual void dragEnterEvent(QDragEnterEvent *ev);

private:
    QPointF translatePoint(int x, int y)
    {
        static const QSizeF baseSize(160.0f, 220.0f);
        float wFac = width() / baseSize.width();
        float hFac = height() / baseSize.height();
        return QPoint(int(x * wFac), int(y * hFac));
    }
    
private:
    CCardTable &mCards;
    CCard mCard;
    QPixmap mTitleIcon;
    QPointF *mLastLeftClickPos;    
    QPushButton *mLockButton;
};

#endif // CCARDLABEL_H
