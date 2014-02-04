#ifndef CXMLSAXVALUE_H
#define CXMLSAXVALUE_H

template <class TTagType = void*> class CXmlSaxValue
{
public:
    CXmlSaxValue(const QString &tag)
        : mTag(tag)
        , mDefaultValue(0)
        , mValue(0)
        , mIsOpen(false)
    { }
    CXmlSaxValue(const QString &tag, const TTagType &value)
        : mTag(tag)
        , mDefaultValue(value)
        , mValue(value)
        , mIsOpen(false)
    { }
    bool handleStartElement(const QString &tag)
    {
        bool open(mTag.compare(tag) == 0);
        if (!mIsOpen && open)
        {
            mIsOpen = true;
            return true;
        }
        return false;
    }
    bool handleEndElement(const QString &tag)
    {
        bool close(mTag.compare(tag) == 0);
        if (mIsOpen && close)
        {
            mIsOpen = false;
            return true;
        }
        return false;
    }
    void reset()
    {
        mValue = mDefaultValue;
        mIsOpen = false;
    }
    void setValue(const TTagType &value)
    {
        mValue = value;
    }
    bool isOpen() const
    {
        return mIsOpen;
    }
    const TTagType& value() const
    {
        return mValue;
    }
private:
    QString mTag;
    TTagType mDefaultValue;
    TTagType mValue;
    bool mIsOpen;
};

#endif // CXMLSAXVALUE_H
