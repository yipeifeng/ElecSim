#include "ElecDataStruct/Pulse.h"
#include "Context/TimeStamp.h"


double Pulse::amplitude(){
    return m_amplitude;
}


TimeStamp Pulse::pulseHitTime(){
    return m_pulseHitTime;
}


TimeStamp Pulse::pulseHitTime() const{
    return m_pulseHitTime;
}


TimeStamp Pulse::EvtTimeStamp(){
    return m_EvtTimeStamp;
}


int Pulse::pmtID(){
    return m_pmtID;
}





