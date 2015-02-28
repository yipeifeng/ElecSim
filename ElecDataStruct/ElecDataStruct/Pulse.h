#ifndef Pulse_h
#define Pulse_h
#include "Context/TimeStamp.h"


class Pulse {

    public:
        Pulse(int o_pmtID,
                double o_amplitude,
                TimeStamp o_pulseHitTime,
                TimeStamp o_EvtTimeStamp)
            :
                m_pmtID(o_pmtID),
                m_amplitude(o_amplitude),
                m_pulseHitTime(o_pulseHitTime),
                m_EvtTimeStamp(o_EvtTimeStamp)
    {}

        Pulse(const Pulse& others)
            :
                m_amplitude(others.m_amplitude),
                m_pulseHitTime(others.m_pulseHitTime),
                m_pmtID(others.m_pmtID),
                m_EvtTimeStamp(others.m_EvtTimeStamp)
    {}

        double amplitude();
        TimeStamp pulseHitTime();
        TimeStamp EvtTimeStamp();
        int pmtID();

    private:

        double m_amplitude;
        TimeStamp m_pulseHitTime;
        TimeStamp m_EvtTimeStamp;
        int m_pmtID;

};




#endif
