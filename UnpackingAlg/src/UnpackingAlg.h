#ifndef UnpackingAlg_h
#define UnpackingAlg_h
#include "SniperKernel/AlgBase.h"
#include "EvtNavigator/NavBuffer.h"
#include "Context/TimeStamp.h"
#include "ElecBufferMgrSvc/IElecBufferMgrSvc.h"
#include "GlobalTimeSvc/IGlobalTimeSvc.h"
#include <map>
#include <vector>
#include <string>


namespace JM 
{
    class SimHeader;
    class SimEvent;
}


class UnpackingAlg: public AlgBase
{
    public:
        UnpackingAlg(const std::string& name);
        ~UnpackingAlg();

        bool initialize();
        bool execute();
        bool finalize();

    private:

        bool init_variable();
        bool get_BufferMgrSvc();


        bool load_event_data();
        bool put_data_to_HitBuffer();
        bool sort_buffer();





    private:
        JM::SimHeader* m_simheader;
        JM::SimEvent* m_simevent;
        double m_nPhotons;
        TTimeStamp m_current_evt_TimeStamp;

        IElecBufferMgrSvc* BufferSvc;
        IGlobalTimeSvc* TimeSvc; 

};















#endif
