#ifndef ReadOutAlg_h
#define ReadOutAlg_h
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


class ReadOutAlg: public AlgBase
{
    public:
        ReadOutAlg(const std::string& name);
        ~ReadOutAlg();

        bool initialize();
        bool execute();
        bool finalize();

    private:

        bool get_Services();
        bool put_hit_into_buffer();



    private:
    int m_evtID;
    IElecBufferMgrSvc* BufferSvc;
    IGlobalTimeSvc* TimeSvc;




};




#endif
