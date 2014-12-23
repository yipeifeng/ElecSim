#ifndef Hit_h
#define Hit_h
#include "Context/TimeStamp.h"



class Hit{

    public:
        Hit(int i_pmtID,
                TimeStamp i_hitTime,
                TimeStamp i_EvtTimeStamp,
                double i_weight):
            m_pmtID(i_pmtID),
            m_hitTime(i_hitTime),
            m_EvtTimeStamp(i_EvtTimeStamp),
            m_weight(i_weight)
    {}


    public:
        int pmtID();
        TimeStamp hitTime();
        TimeStamp EvtTimeStamp();
        double weight();
        double relative_hitTime_ns();







    private:

        int m_pmtID;
        TimeStamp m_hitTime;
        TimeStamp m_EvtTimeStamp;
        double m_weight;
};





#endif
