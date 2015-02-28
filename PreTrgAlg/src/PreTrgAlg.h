#ifndef PreTrgAlg_h
#define PreTrgAlg_h
#include "SniperKernel/AlgBase.h"
#include "EvtNavigator/NavBuffer.h"
#include "Context/TimeStamp.h"
#include "ElecBufferMgrSvc/IElecBufferMgrSvc.h"
#include "GlobalTimeSvc/IGlobalTimeSvc.h"
#include <map>
#include <vector>
#include <string>


class PreTrgAlg: public AlgBase
{
    public:
        PreTrgAlg(const std::string& name);
        ~PreTrgAlg();

        bool initialize();
        bool execute();
        bool finalize();
        bool find_Trg_from_PulseBuffer();
        bool get_Services();



    private:

        IElecBufferMgrSvc* BufferSvc;
        IGlobalTimeSvc* TimeSvc;


        double m_PulseBufferLength; //unit ns

};




















#endif
