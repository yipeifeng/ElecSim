#include "GlobalTimeSvc.h"
#include "SniperKernel/SvcFactory.h"
#include "SniperKernel/SniperLog.h"

using namespace std;

DECLARE_SERVICE(GlobalTimeSvc);


GlobalTimeSvc::GlobalTimeSvc(const std::string& name)
    : SvcBase(name)
{
    declProp("start", m_start_Time);
}


GlobalTimeSvc::~GlobalTimeSvc(){

}


bool GlobalTimeSvc::initialize(){

    LogInfo<<"I'm at Beijing , so I input local time 1970-1-1 08:00:00. It equal standard time 1970-1-1 00:00:00"<<endl;
    LogInfo<<"start Time (local time): " << m_start_Time<<endl;
    current_evt_time = str2time(m_start_Time);
    LogInfo<<"current_evt_time: "  << current_evt_time<<endl;



    return true;

}





bool GlobalTimeSvc::finalize(){


    return true;
}


TTimeStamp GlobalTimeSvc::get_current_evt_time(TTimeStamp delta){

    current_evt_time.Add(delta);  
    return current_evt_time;

}



TTimeStamp GlobalTimeSvc::str2time(string str_time)
{
    struct tm tm_time;
    strptime(str_time.c_str(),"%Y-%m-%d %H:%M:%S", &tm_time);
    time_t tTime=mktime(&tm_time);
    LogInfo<<"tTime (number of seconds since standard time 1970-1-1 00:00:00): " << tTime<<endl;


    TTimeStamp answer(tTime,0);
    return answer;
}



