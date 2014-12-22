#include "RootRandomSvc.h"
#include "SniperKernel/SvcFactory.h"
#include "SniperKernel/SniperLog.h"
#include <TRandom.h>

DECLARE_SERVICE(RootRandomSvc);

RootRandomSvc::RootRandomSvc(const std::string& name)
    : SvcBase(name)
{
    declProp("Seed", m_init_seed);
}

RootRandomSvc::~RootRandomSvc(){

}


bool RootRandomSvc::initialize(){

    LogDebug<<"Initialize the SEED = "
            <<m_init_seed
            <<std::endl;

    setSeed(m_init_seed); //when initialize,we set seed.so in python script just need create service.

    return true;

}


bool RootRandomSvc::finalize(){
    LogDebug << "Finalize the SEED="
             << gRandom->GetSeed()
             << std::endl;
    return true;
}


void RootRandomSvc::setSeed(int seed){

    gRandom->SetSeed(seed);
}








