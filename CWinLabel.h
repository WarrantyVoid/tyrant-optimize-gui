#ifndef CWINLABEL_H
#define CWINLABEL_H

#include <QLabel>
#include <QDropEvent>

class CWinLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CWinLabel(QWidget *parent = 0);
    
signals:
    void unitDropped();

protected:
    virtual void dropEvent(QDropEvent *ev);
    virtual void dragMoveEvent(QDragMoveEvent *ev);
    virtual void dragEnterEvent(QDragEnterEvent *ev);
    
};

#endif // CWINLABEL_H
