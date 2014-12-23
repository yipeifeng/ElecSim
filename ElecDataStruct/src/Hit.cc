#include "ElecDataStruct/Hit.h"
#include "Context/TimeStamp.h"


int Hit::pmtID(){
    return m_pmtID; 
}


TimeStamp Hit::hitTime(){
    return m_hitTime;
}


TimeStamp Hit::EvtTimeStamp(){
    return m_EvtTimeStamp;
}

double Hit::weight(){
    return m_weight;
}


double Hit::relative_hitTime_ns(){
    double relative_hitTime_secnod =  (m_hitTime - m_EvtTimeStamp).GetSeconds();
    double relative_hitTime_nanosecond = relative_hitTime_secnod * 1e9;
    return relative_hitTime_nanosecond;
}










