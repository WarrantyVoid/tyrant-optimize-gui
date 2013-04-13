#ifndef CQUESTSXMLPARSER_H
#define CQUESTSXMLPARSER_H

#include <QXmlDefaultHandler>
#include "model/CBattleground.h"

class CQuestsXmlParser : public QObject, public QXmlDefaultHandler
{
    Q_OBJECT

public:
    CQuestsXmlParser();

signals:
    void battlegroundParsed(const CBattleground &battleground);

protected:
    virtual bool startDocument();
    virtual bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts);
    virtual bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName);
    virtual bool characters(const QString & ch);
    virtual bool endDocument();

private:
    bool mIsBattlegroundTagActive;
    bool mIsNameTagActive;
    bool mIsDescTagActive;
    bool mIsIconTagActive;
    CBattleground mCurBattleground;
};

#endif // CQUESTSXMLPARSER_H
