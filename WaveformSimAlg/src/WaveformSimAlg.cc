#include "WaveformSimAlg.h"
#include "SniperKernel/AlgFactory.h"
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
#include "ElecBufferMgrSvc/IElecBufferMgrSvc.h"
#include "ElecDataStruct/Hit.h"


using namespace std;

DECLARE_ALGORITHM(WaveformSimAlg);


WaveformSimAlg::WaveformSimAlg(const string& name):AlgBase(name){


}

WaveformSimAlg::~WaveformSimAlg(){

}





bool WaveformSimAlg::initialize(){


    return true;

}



bool WaveformSimAlg::execute(){

    LogInfo<<"execute WaveformSimAlg! "<<endl;

    return true;
}


bool WaveformSimAlg::finalize(){


    return true;
}














