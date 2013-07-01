#ifndef CMISSIONSXMLPARSER_H
#define CMISSIONSXMLPARSER_H

#include <QXmlDefaultHandler>
#include "model/CDeck.h"

class CMissionsXmlParser : public QObject, public QXmlDefaultHandler
{

    Q_OBJECT

public:
    CMissionsXmlParser();

signals:
    void missionParsed(unsigned int id, const QString &missionName, EDeckType type, unsigned int battlegroundId, const QList<unsigned int> &missionDeck);

protected:
    virtual bool startDocument();
    virtual bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts);
    virtual bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName);
    virtual bool characters(const QString & ch);
    virtual bool endDocument();

private:
    bool mIsMissionTagActive;
    bool mIsNameTagActive;
    bool mIdTagActive;
    bool mIsCommanderTagActive;
    bool mIsDeckTagActive;
    bool mIsCardTagActive;
    QString mCurMissionName;
    unsigned int mCurMissionId;
    QList<unsigned int> mCurMissionDeck;
};

#endif // CMISSIONSXMLPARSER_H
