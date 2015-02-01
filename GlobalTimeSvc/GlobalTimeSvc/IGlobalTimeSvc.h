#ifndef IGlobalTimeSvc_h
#define IGlobalTimeSvc_h
#include "Context/TimeStamp.h"
#include <TTimeStamp.h>



class IGlobalTimeSvc{
    public:
        virtual ~IGlobalTimeSvc() = 0;
        virtual TTimeStamp set_current_evt_time(TTimeStamp delta) = 0;
        virtual TTimeStamp get_current_evt_time() = 0;
        
        virtual TimeStamp get_start_time() = 0;



};








#endif
