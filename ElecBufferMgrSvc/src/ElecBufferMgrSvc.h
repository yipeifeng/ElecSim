#ifndef ElecBufferMgrSvc_h
#define ElecBufferMgrSvc_h

#include "SniperKernel/SvcBase.h"
#include "ElecBufferMgrSvc/IElecBufferMgrSvc.h"
#include "ElecDataStruct/Hit.h"
#include "ElecDataStruct/Pulse.h"
#include "Context/TimeStamp.h"
#include <deque>
#include <vector>

class ElecBufferMgrSvc: public IElecBufferMgrSvc, public SvcBase
{
    public:
        ElecBufferMgrSvc(const std::string& name);
        ~ElecBufferMgrSvc();

        bool initialize();
        bool finalize();

//Hit Buffer
        std::deque<Hit>& get_HitBuffer();
        void save_to_HitBuffer(Hit hit);
        void SortHitBuffer();

        TimeStamp get_firstHitTime(); //get first hit time in HitBuffer
        TimeStamp get_lastHitTime(); //get last hit time in HitBuffer
        int get_HitBufferSize(); 

        std::vector<Hit> get_HitVector(double TimeLength);


//Pulse Buffer
        std::deque<Pulse>& get_PulseBuffer();
        TimeStamp get_firstPulseTime();
        TimeStamp get_lastPulseTime();

        void save_to_PulseBuffer(Pulse pulse);

        int get_PulseBufferSize();

        void SortPulseBuffer();

        void pop_PulseBufferFront();

        std::vector<Pulse> get_PulseVector(TimeStamp WaveSimLastTime);




//Trigger Buffer
        std::deque<TimeStamp>& get_TriggerBuffer();
        void save_to_TriggerBuffer(TimeStamp TriggerTime);

        TimeStamp get_TriggerTimeStamp();//return the first TriggerTime .

        void pop_TriggerTimeStamp(); //
        
        


    private:
        std::deque<Hit> HitBuffer;
        std::deque<Pulse> PulseBuffer;
        std::deque<TimeStamp> TriggerBuffer;



};












#endif
