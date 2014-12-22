#ifndef GlobalTimeSvc_h
#define GlobalTimeSvc_h

#include "SniperKernel/SvcBase.h"
#include "GlobalTimeSvc/IGlobalTimeSvc.h"
#include <TTimeStamp.h>
#include <string>

class GlobalTimeSvc: public IGlobalTimeSvc, public SvcBase
{
    public:
        GlobalTimeSvc(const std::string& name);
        ~GlobalTimeSvc();

        bool initialize();
        bool finalize();

        TTimeStamp get_current_evt_time(TTimeStamp delta);



    private:
        std::string m_start_Time;
        TTimeStamp current_evt_time;
        
        std::map<std::string,double> m_rateMap;///events rate map
        double m_totalRate;///the sum of events rate
        double m_mainTau;///1.0/totalRate









    


    private:

        TTimeStamp str2time(std::string str_time);




};


#endif
