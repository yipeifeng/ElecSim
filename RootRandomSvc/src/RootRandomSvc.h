#ifndef RootRandomSvc_h
#define RootRandomSvc_h

#include "SniperKernel/SvcBase.h"
#include "RootRandomSvc/IRootRandomSvc.h" 

class RootRandomSvc: public IRootRandomSvc, public SvcBase
{
    public:
        RootRandomSvc(const std::string& name);
        ~RootRandomSvc();

        bool initialize();
        bool finalize();
        
        void setSeed(int seed);

    private:
        int m_init_seed;

};






#endif
