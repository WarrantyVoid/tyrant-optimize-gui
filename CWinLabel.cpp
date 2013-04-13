#include "CWinLabel.h"

CWinLabel::CWinLabel(QWidget *parent)
: QLabel(parent)
{
}

void CWinLabel::dropEvent(QDropEvent *ev)
{
    QString data = ev->mimeData()->text().toLatin1();
    if (data.length() < 50)
    {
        QStringList valueList = data.split("=");
        if (valueList.size() == 2)
        {
            bool ok(true);
            valueList.at(1).toInt(&ok);
            if (ok)
            {
                ev->accept();
                emit unitDropped();
            }
        }
    }
}

void CWinLabel::dragMoveEvent(QDragMoveEvent *ev)
{
    ev->accept();
}

void CWinLabel::dragEnterEvent(QDragEnterEvent *ev)
{
    ev->acceptProposedAction();
}
