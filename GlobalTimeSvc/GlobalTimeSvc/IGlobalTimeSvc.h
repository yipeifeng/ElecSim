#ifndef IGlobalTimeSvc_h
#define IGlobalTimeSvc_h
#include <TTimeStamp.h>


class IGlobalTimeSvc{
    public:
        virtual ~IGlobalTimeSvc() = 0;
        virtual TTimeStamp get_current_evt_time(TTimeStamp delta) = 0;
        



};








#endif
