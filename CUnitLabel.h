#ifndef CUNITLABEL_H
#define CUNITLABEL_H

#include <QLabel>
#include <QDropEvent>
#include "CCardTable.h"

class CUnitLabel : public QLabel
{
    Q_OBJECT

public:
    explicit CUnitLabel(QWidget *parent = 0);

public:
    void setCard(const CCard& card);
    const CCard& getCard() const;

signals:
    void unitDropped();

protected:
    virtual void paintEvent(QPaintEvent *ev);
    virtual void mousePressEvent(QMouseEvent * ev);
    virtual void mouseReleaseEvent(QMouseEvent * ev);
    virtual void mouseMoveEvent(QMouseEvent * ev);
    virtual void dropEvent(QDropEvent *ev);
    virtual void dragMoveEvent(QDragMoveEvent *ev);
    virtual void dragEnterEvent(QDragEnterEvent *ev);
    
private:
    CCardTable &mCards;
    CCard mCard;
    QPixmap mTitleIcon;
    QPointF *mLastLeftClickPos;    
};

#endif // CUNITLABEL_H
