#include "CPictureDownload.h"

CPictureDownload::CPictureDownload(const QString &urlName, const QString &fileName)
: CDownload(urlName, fileName)
, mMetaData()
{        
}

void CPictureDownload::setPictureMetaData(const CCard &metaData)
{
    mMetaData = metaData;
}

const CCard& CPictureDownload::getPictureMetaData() const
{
    return mMetaData;
}

