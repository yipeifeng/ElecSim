#ifndef ReadOutAlg_h
#define ReadOutAlg_h
#include "SniperKernel/AlgBase.h"
#include "EvtNavigator/NavBuffer.h"
#include "Context/TimeStamp.h"
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



    private:
    int m_evtID;

};




#endif
