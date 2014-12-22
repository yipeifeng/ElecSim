#ifndef EvtMixingAlg_h
#define EvtMixingAlg_h
#include "SniperKernel/AlgBase.h"
#include "EvtNavigator/NavBuffer.h"
#include "Context/TimeStamp.h"
#include <map>
#include <vector>
#include <string>


class IDataMemMgr;
class InputReviser;

namespace JM 
{
    class SimHeader;
    class SimEvent;
}



class EvtMixingAlg: public AlgBase
{


    public:
        EvtMixingAlg(const std::string& name);
        ~EvtMixingAlg();

        bool initialize();
        bool execute();
        bool finalize();

    private:

        //get one event from samples
        JM::EvtNavigator* get_one_event();

        //set TimeStamp to a event and put it to sniper buffer
        void package_new_event(JM::EvtNavigator* Nav, TTimeStamp EvtTimeStamp); 

        ///select sample from set of samples
        std::string select();

    private:

        std::map<std::string,double> m_rateMap;///events rate map
        double m_totalRate;///the sum of events rate
        double m_mainTau;///1.0/totalRate

        ///sniper related
        IDataMemMgr* m_memMgr_top;
        std::map<std::string, std::string> m_taskMap;
        std::map<std::string, InputReviser*> m_incidentMap;

        //used to decide first entry's position
        std::map<std::string, bool> m_firstMap;


};












#endif
