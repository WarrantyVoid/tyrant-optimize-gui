#ifndef CACHIEVEMENTSXMLPARSER_H
#define CACHIEVEMENTSXMLPARSER_H

#include <QXmlDefaultHandler>
#include "model/CAchievement.h"

class CAchievementsXmlParser : public QObject, public QXmlDefaultHandler
{
    Q_OBJECT

public:
    CAchievementsXmlParser();

signals:
    void achievementParsed(const CAchievement &achievement);

protected:
    virtual bool startDocument();
    virtual bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts);
    virtual bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName);
    virtual bool characters(const QString & ch);
    virtual bool endDocument();

private:
    bool mIsAchievementTagActive;
    bool mIsIdTagActive;
    bool mIsNameTagActive;
    bool mIsDescTagActive;
    CAchievement mCurAchievement;
};

#endif // CACHIEVEMENTSXMLPARSER_H
