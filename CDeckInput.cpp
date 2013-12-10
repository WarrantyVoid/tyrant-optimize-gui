#include "CDeckInput.h"
#include <QLineEdit>
#include <QMimeData>

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

QString CDeckInput::getDeckId() const
{
    return currentText();
}

QStringList CDeckInput::getHistory() const
{
    QStringList history;
    for(int i = 0; i < count(); ++i)
    {
        history.push_back(itemText(i));
    }
    return history;
}

QMimeData *CDeckInput::createDeckInputDropData(const QString &deckId)
{
    QMimeData *data = new QMimeData();
    data->setHtml(QString("@CDeckInput:id=%1").arg(deckId));
    return data;
}

bool CDeckInput::isDeckInputDropData(const QMimeData *data, bool isMultiAllowed)
{
    if (data && data->hasHtml())
    {
        QString hmtmlData = data->html().toLatin1();
        if (hmtmlData.length() < 1024 && hmtmlData.startsWith("@CDeckInput:id="))
        {
            return isMultiAllowed || hmtmlData.indexOf(";") == -1;
        }
    }
    return false;
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
            QDropEvent *ev = static_cast<QDropEvent *>(e);
            if (ev && CDeckInput::isDeckInputDropData(ev->mimeData(), mAreMultiDecksAllowed))
            {
                QString data = ev->mimeData()->html().toLatin1();
                int dataIdx = data.indexOf("=");
                if (dataIdx > -1)
                {
                    data = data.mid(dataIdx + 1);
                    if (data.indexOf(";") == -1 && lineEdit()->text().endsWith(";"))
                    {
                        lineEdit()->setText(lineEdit()->text() + data);
                    }
                    else
                    {
                        lineEdit()->setText(data);
                    }
                    lineEdit()->setFocus();
                    emit deckDropped(data);
                    ev->accept();
                }
            }
            return true;
        }
        case QEvent::DragMove:
        {
            QDragMoveEvent *ev = static_cast<QDragMoveEvent *>(e);
            if (ev && CDeckInput::isDeckInputDropData(ev->mimeData(), mAreMultiDecksAllowed))
            {
                ev->accept();
            }
            return true;
        }
        case QEvent::DragEnter:
        {
            QDragEnterEvent *ev = static_cast<QDragEnterEvent *>(e);
            if (ev && CDeckInput::isDeckInputDropData(ev->mimeData(), mAreMultiDecksAllowed))
            {
                ev->acceptProposedAction();
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
