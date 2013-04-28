#ifndef ICARDCHECK
#define ICARDCHECK

class CCard;

class ICardCheck
{
public:
    virtual ~ICardCheck() { }

public:
    virtual bool checkCard(const CCard &card, int &num) const = 0;
};

#endif // ICARDCHECK
