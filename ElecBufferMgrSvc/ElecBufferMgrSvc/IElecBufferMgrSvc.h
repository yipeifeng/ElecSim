#ifndef IElecBufferMgrSvc_h
#define IElecBufferMgrSvc_h
#include "ElecDataStruct/Hit.h"
#include "ElecDataStruct/Pulse.h"
#include "Context/TimeStamp.h"
#include <deque>



class IElecBufferMgrSvc{

    public:
        virtual ~IElecBufferMgrSvc() = 0;

//Hit Buffer        

        virtual void save_to_HitBuffer(Hit hit) = 0; 
        virtual std::deque<Hit>& get_HitBuffer() =0;
        virtual void SortHitBuffer() = 0;

        virtual TimeStamp get_firstHitTime() = 0;//get first hit time in HitBuffer
        virtual TimeStamp get_lastHitTime() = 0;//get first hit time in HitBuffer
        virtual int get_HitBufferSize() = 0;

        virtual std::vector<Hit> get_HitVector(double TimeLength) = 0;


//Trigger Buffer

        virtual std::deque<TimeStamp>& get_TriggerBuffer() = 0;//get trigger buffer

        virtual void save_to_TriggerBuffer(TimeStamp TriggerTime) = 0;


        virtual TimeStamp get_TriggerTimeStamp() = 0;

        virtual void pop_TriggerTimeStamp() = 0;


        //Pulse Buffer

        virtual std::deque<Pulse>& get_PulseBuffer() = 0;
        virtual TimeStamp get_firstPulseTime() = 0;
        virtual TimeStamp get_lastPulseTime() = 0;
        virtual void save_to_PulseBuffer(Pulse pulse) = 0;

        virtual int get_PulseBufferSize() = 0;

        virtual void SortPulseBuffer() = 0;

        virtual void pop_PulseBufferFront() = 0;

        virtual std::vector<Pulse> get_PulseVector(TimeStamp WaveSimLastTime) = 0;


        //Waveform Buffer

        virtual void save_waveform(int channelId, TimeStamp index_stamp, double amplitude) = 0;

        virtual void set_standard_TimeStamp(TimeStamp time) = 0;

};













#endif
