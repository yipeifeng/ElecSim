#ifndef ElecBufferMgrSvc_h
#define ElecBufferMgrSvc_h

#include "SniperKernel/SvcBase.h"
#include "ElecBufferMgrSvc/IElecBufferMgrSvc.h"
#include "ElecDataStruct/Hit.h"
#include "ElecDataStruct/Pulse.h"
#include "Context/TimeStamp.h"
#include "Event/SimHeader.h"
#include "Event/ElecHeader.h"
//#include "EvtNavigator/NavBuffer.h"
//#include "BufferMemMgr/IDataMemMgr.h"
//#include "DataRegistritionSvc/DataRegistritionSvc.h"
//#include "Identifier/CdID.h"

#include <deque>
#include <vector>

//class for channel data class

class ChannelData{

    public:
        ChannelData();
        ChannelData(int BufferSize);
        ~ChannelData();

        std::vector<double>& ChannelBuffer();
        void save_value(int BufferSize, TimeStamp standard_TimeStamp, int  standard_Index, TimeStamp ValueTime, double ValueAmp);


    private:

    std::vector<double> m_ChannelBuffer;


};









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
        
        

//Waveform Buffer

        void save_waveform(int channelId, TimeStamp index_stamp, double amplitude);
        void set_standard_TimeStamp(TimeStamp time);



//output crate

        JM::ElecFeeCrate* get_crate(); 
        void create_new_crate();
        void delete_crate();



    private:
        int m_PmtTotal;
        int m_WaveformBufferSize;

        TimeStamp standard_TimeStamp;
        int standard_Index;



        std::deque<Hit> HitBuffer;
        std::deque<Pulse> PulseBuffer;
        std::deque<TimeStamp> TriggerBuffer;


        //Waveform Buffer

        std::map<int, ChannelData> WaveformBuffer;



        void init_WavefromBuffer(int PmtTotal);


        //output crate
        JM::ElecFeeCrate *m_crate;


};












#endif
