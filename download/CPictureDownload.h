#ifndef CCARDDOWNLOAD_H
#define CCARDDOWNLOAD_H

#include "CDownload.h"
#include "model/CCard.h"

class CPictureDownload : public CDownload
{
    Q_OBJECT

public:
    CPictureDownload(const QString &urlName, const QString &fileName);

public:
    void setPictureMetaData(const CCard &metaData);
    const CCard& getPictureMetaData() const;

private:
    CCard mMetaData;
};

#endif // CCARDDOWNLOAD_H
