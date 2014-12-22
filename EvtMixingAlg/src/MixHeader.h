//--------------------------------------
//MixHeader.h
//author: HxWang
//Email: ryan.hxwang@gmail.com
//2014,08,19
//--------------------------------------

#ifndef MixHeader_hh
#define MixHeader_hh

#include "Context/TimeStamp.h"

class MixHeader
{
    friend bool operator < (const MixHeader& a, const MixHeader& b);

    public:

    MixHeader();
    MixHeader(TimeStamp& time, int pmtId);
    ~MixHeader();

    void setTimeStamp(TimeStamp& value);
    TimeStamp getTimeStamp() const;

    void setPmtID(int value);
    int getPmtID();

    private:

    TimeStamp m_timeStamp;
    int m_pmtID;
};


inline MixHeader::MixHeader(){}


inline MixHeader::MixHeader(TimeStamp& time, int pmtId)
{
    m_timeStamp = time;
    m_pmtID = pmtId;
}


inline MixHeader::~MixHeader(){}


inline void MixHeader::setTimeStamp(TimeStamp& value)
{
    m_timeStamp = value;
}


inline TimeStamp MixHeader::getTimeStamp() const
{
    return m_timeStamp;
}


inline void MixHeader::setPmtID(int value)
{
    m_pmtID = value;
}

inline int MixHeader::getPmtID()
{
    return m_pmtID;
}


inline bool operator < (const MixHeader& a, const MixHeader& b)
{
    if(a.getTimeStamp() < b.getTimeStamp()) return true;
    else return false;
}


#endif


