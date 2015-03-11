#ifndef WaveformSimAlg_h
#define WaveformSimAlg_h
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


class WaveformSimAlg: public AlgBase
{
    public:
        WaveformSimAlg(const std::string& name);
        ~WaveformSimAlg();

        bool initialize();
        bool execute();
        bool finalize();


    private:
        void get_Services();
        void clear_vector();
        void load_Pulse();






    private:

        JM::SimHeader* m_simheader;
        JM::SimEvent* m_simevent;

        IElecBufferMgrSvc* BufferSvc;
        IGlobalTimeSvc* TimeSvc; 

        std::vector<Pulse> pulse_vector;

        double m_preWaveSimWindow;
        double m_postWaveSimWindow;

        

};





#endif
