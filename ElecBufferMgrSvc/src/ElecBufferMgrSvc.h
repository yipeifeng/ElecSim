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






        std::vector<TimeStamp>& get_TriggerBuffer();




    private:
        std::deque<Hit> HitBuffer;
        std::deque<Pulse> PulseBuffer;
        std::vector<TimeStamp> TriggerBuffer;



};












#endif
