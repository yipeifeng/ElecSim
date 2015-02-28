#ifndef IElecBufferMgrSvc_h
#define IElecBufferMgrSvc_h
#include "ElecDataStruct/Hit.h"
#include "ElecDataStruct/Pulse.h"
#include "Context/TimeStamp.h"
#include <deque>



class IElecBufferMgrSvc{

    public:
        virtual ~IElecBufferMgrSvc() = 0;
        
        virtual void save_to_HitBuffer(Hit hit) = 0; 
        virtual std::deque<Hit>& get_HitBuffer() =0;
        virtual void SortHitBuffer() = 0;

        virtual TimeStamp get_firstHitTime() = 0;//get first hit time in HitBuffer
        virtual TimeStamp get_lastHitTime() = 0;//get first hit time in HitBuffer
        virtual int get_HitBufferSize() = 0;

        virtual std::vector<Hit> get_HitVector(double TimeLength) = 0;



        virtual std::vector<TimeStamp>& get_TriggerBuffer() = 0;//get trigger buffer

//Pulse Buffer

        virtual std::deque<Pulse>& get_PulseBuffer() = 0;
        virtual TimeStamp get_firstPulseTime() = 0;
        virtual TimeStamp get_lastPulseTime() = 0;
        virtual void save_to_PulseBuffer(Pulse pulse) = 0;

        virtual int get_PulseBufferSize() = 0;

};













#endif
