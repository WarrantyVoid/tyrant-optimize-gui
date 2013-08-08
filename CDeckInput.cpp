#include "CDeckInput.h"
#include <QLineEdit>

CDeckInput::CDeckInput(QWidget *parent)
: QComboBox(parent)
, mAreMultiDecksAllowed(false)
{
    setEditable(true);
    lineEdit()->installEventFilter(this);
}

void CDeckInput::setAllowMultiDecks(bool allow)
{
    mAreMultiDecksAllowed = allow;
}

QStringList CDeckInput::history() const
{
    QStringList history;
    for(int i = 0; i < count(); ++i)
    {
        history.push_back(itemText(i));
    }
    return history;
}

void CDeckInput::setDeckId(const QString &deckStr)
{
    lineEdit()->setText(deckStr);
}

void CDeckInput::updateHistory(const QStringList &history)
{
    clear();
    addItems(history);
}

void CDeckInput::updateHistory()
{
    QString deckStr = lineEdit()->text().trimmed();
    if (!deckStr.isEmpty())
    {
        int idx = findText(deckStr);
        if (idx > -1)
        {
            removeItem(idx);
        }
        insertItem(0, deckStr);
        setCurrentIndex(0);
    }
}

bool CDeckInput::eventFilter(QObject *obj, QEvent *e)
{
    switch (e->type())
    {
        case QEvent::Drop:
        {
            QDropEvent *event = static_cast<QDropEvent *>(e);
            QString data = event->mimeData()->text().toLatin1();
            if (data.length() < 1024)
            {
                if (data.indexOf(";") == -1 || mAreMultiDecksAllowed)
                {
                    lineEdit()->setText(data);
                    lineEdit()->setFocus();
                    emit deckDropped(data);
                }
            }
            event->accept();
            return true;
        }
        case QEvent::DragMove:
        {
            QDragMoveEvent *event = static_cast<QDragMoveEvent *>(e);
            QString data = event->mimeData()->text().toLatin1();
            if (data.length() < 1024)
            {
                if (data.indexOf(";") == -1 || mAreMultiDecksAllowed)
                {
                    event->accept();
                }
            }
            return true;
        }
        case QEvent::DragEnter:
        {
            QDragEnterEvent *event = static_cast<QDragEnterEvent *>(e);
            QString data = event->mimeData()->text().toLatin1();
            if (data.length() < 1024)
            {
                if (data.indexOf(";") == -1 || mAreMultiDecksAllowed)
                {
                    event->acceptProposedAction();
                }
            }
            return true;
        }
        default:
        {
            // standard event processing
            return QObject::eventFilter(obj, e);
        }
    }
}
