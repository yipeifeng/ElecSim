#include "UnpackingAlg.h"
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
#include "ElecDataStruct/Hit.h"


using namespace std;

DECLARE_ALGORITHM(UnpackingAlg);


UnpackingAlg::UnpackingAlg(const string& name):AlgBase(name){


}

UnpackingAlg::~UnpackingAlg(){

}





bool UnpackingAlg::initialize(){
    m_nPhotons = 0;
    m_current_evt_TimeStamp = 0;
    return true;

}



bool UnpackingAlg::execute(){

    load_event_data(); 
    put_data_to_HitBuffer();


    return true;
}


bool UnpackingAlg::finalize(){


    return true;
}




bool UnpackingAlg::load_event_data(){

    JM::NavBuffer* navBuf = 0;
    Incident::fire("EvtMixingTask");

    SniperDataPtr<JM::NavBuffer>  navBufPtr("EvtMixingTask:/Event");

    if (navBufPtr.invalid()) {
        return false;
    }

    navBuf = navBufPtr.data();
    if (navBuf->size() == 0) {
        LogInfo << "There is nothing in Cur Buffer." << std::endl;
        return false;
    }
    JM::EvtNavigator* evt_nav = navBuf->curEvt();
    if (not evt_nav) {
        return false;
    }
    m_simheader = dynamic_cast<JM::SimHeader*>(evt_nav->getHeader("/Event/SimEvent"));
    LogDebug << "simhdr: " << m_simheader << std::endl;
    if (not m_simheader) {
        return false;
    }
    m_simevent = dynamic_cast<JM::SimEvent*>(m_simheader->event());
    LogDebug << "simevt: " << m_simevent << std::endl;
    if (not m_simevent) {
        return false;
    }

    m_current_evt_TimeStamp = evt_nav->TimeStamp();
    LogInfo<<"load evt TimeStamp: "<<m_current_evt_TimeStamp<<endl;

    return true;

}


bool UnpackingAlg::put_data_to_HitBuffer(){
    m_nPhotons = 0;

    assert(m_simevent);
    JM::SimPMTHit* hit = 0;
    TIter next_hit(m_simevent->getCDHits());
    while( (hit = (JM::SimPMTHit*)next_hit()) ) {
        int hit_pmtId = hit->getPMTID();
        //cout<<"hit_pmtID: " << hit_pmtId<<endl;
        double hit_hitTime = hit->getHitTime();
        
        //LogInfo<<"hit Time: " << hit_hitTime<<endl;
        // FIXME
        // if we get a merged hit, what's the nPhotons???
        
        TimeStamp EvtTimeStamp(m_current_evt_TimeStamp.GetSec(), m_current_evt_TimeStamp.GetNanoSec()); // convert TTimeStamp to TimeStamp
        TimeStamp m_hitTime = EvtTimeStamp;  
        m_hitTime.Add(hit_hitTime*1e-9);//convert ns to s ,get the global hitTime

        double m_weight = 1;

        Hit m_hit(hit_pmtId, m_hitTime, EvtTimeStamp, m_weight);
        //LogInfo<<"EvtTimeStamp: " << EvtTimeStamp<<endl;
        //LogInfo<<"relative_hitTime_ns: " <<  m_hit.relative_hitTime_ns()<<endl;



        ++m_nPhotons;

    }
    LogInfo<<"nPhotons: " << m_nPhotons<<endl;

    return true;
}


