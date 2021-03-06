#ifndef CWINLABEL_H
#define CWINLABEL_H

#include <QLabel>
#include "process/IProcessWrapper.h"
#include "process/CProcessParameters.h"

class CWinLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CWinLabel(QWidget *parent = 0);

public:
    void setWinText(const SOptimizationStatus &status, EOptimizationMode mode);
    
signals:
    void unitDropped();
    void trashTriggered();

protected:
    virtual void paintEvent(QPaintEvent *ev);
    virtual void dropEvent(QDropEvent *ev);
    virtual void dragMoveEvent(QDragMoveEvent *ev);
    virtual void dragEnterEvent(QDragEnterEvent *ev);
    virtual void mouseDoubleClickEvent (QMouseEvent *ev);

private:
    QString mWintext;
    QPixmap mWinPic;
};

#endif // CWINLABEL_H
