#include "ReadOutAlg.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperLog.h"
#include "BufferMemMgr/IDataMemMgr.h"
#include "DataRegistritionSvc/DataRegistritionSvc.h"
#include "SniperKernel/Incident.h"
#include "InputReviser/InputReviser.h"
#include "Identifier/CdID.h"
#include "Event/SimHeader.h"
#include "Event/SimEvent.h"
#include "Event/SimPMTHit.h"
#include <time.h>
#include <TMath.h>
#include <TRandom.h>
#include <TStopwatch.h>
#include <TTimeStamp.h>
#include "Context/TimeStamp.h"
#include "SniperKernel/Task.h"
#include "GlobalTimeSvc/IGlobalTimeSvc.h"


using namespace std;

DECLARE_ALGORITHM(ReadOutAlg);


ReadOutAlg::ReadOutAlg(const string& name):AlgBase(name){


}

ReadOutAlg::~ReadOutAlg(){

}


bool ReadOutAlg::initialize(){

    m_evtID=0;

    return true;
}


bool ReadOutAlg::execute(){

    LogInfo<<"begin event: " <<m_evtID<<endl; 

    Incident::fire("UnpackingTask");


    m_evtID++;
    return true;
}


bool ReadOutAlg::finalize(){


    return true;
}











