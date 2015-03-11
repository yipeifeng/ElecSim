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

    declProp("preWaveSimWindow",m_preWaveSimWindow=200); //unit ns
    declProp("postWaveSimWindow",m_postWaveSimWindow=800);

}

WaveformSimAlg::~WaveformSimAlg(){

}





bool WaveformSimAlg::initialize(){

    get_Services();
    return true;

}



bool WaveformSimAlg::execute(){

    LogInfo<<"execute WaveformSimAlg! "<<endl;
    clear_vector();

    load_Pulse();

    return true;
}


bool WaveformSimAlg::finalize(){


    return true;
}


void WaveformSimAlg::clear_vector(){
    LogInfo<<"clear temp pulse_vector"<<endl;
    pulse_vector.clear(); 
}



void WaveformSimAlg::get_Services(){

    SniperPtr<IGlobalTimeSvc> TimeSvcPtr(Task::top(),"GlobalTimeSvc");
    TimeSvc = TimeSvcPtr.data();

    SniperPtr<IElecBufferMgrSvc> BufferSvcPtr(Task::top(),"ElecBufferMgrSvc");
    BufferSvc = BufferSvcPtr.data();


}


void WaveformSimAlg::load_Pulse(){

    //make sure lastPulseTime > WaveSimLastTime
    TimeStamp TriggerTime = BufferSvc->get_TriggerTimeStamp();

    TimeStamp lastPulseTime = BufferSvc->get_lastPulseTime();

    TimeStamp WaveSimLastTime(0);
    WaveSimLastTime = TriggerTime;
    WaveSimLastTime.Add(m_preWaveSimWindow*1e-9);
    WaveSimLastTime.Add(m_postWaveSimWindow*1e-9); //for tolerance, the WaveSimLastTime = TriggerTime + m_preWaveSimWindow + m_postWaveSimWindow; the lastPulseTime must greater than the WaveSimLastTime.


    LogInfo<<"WaveSimLastTime: " <<WaveSimLastTime.GetSeconds()*1e9<<endl;
    LogInfo<<"lastPulseTime: " << lastPulseTime.GetSeconds()*1e9<<endl;



    while(lastPulseTime < WaveSimLastTime){
        LogInfo<<"the lastPulseTime < WaveSimLastTime, Incident::fire PMTSimTask" <<endl;

        Incident::fire("PMTSimTask");
        lastPulseTime = BufferSvc->get_lastPulseTime();
        LogInfo<<"lastPulseTime: " << lastPulseTime.GetSeconds()*1e9<<endl;

    }

    // pop Pulse before WaveSimFirstTime=TriggerTime-m_preWaveSimWindow

    TimeStamp WaveSimFirstTime(0);
    WaveSimFirstTime = TriggerTime;
    WaveSimFirstTime.Subtract(m_preWaveSimWindow*1e-9);
    //WaveSimFirstTime.Add(100*1e-9);

    LogInfo<<"WaveSimFirstTime: " <<WaveSimFirstTime.GetSeconds()*1e9<<endl;

    TimeStamp firstPulseTime = BufferSvc->get_firstPulseTime();

    LogInfo<<"firstPulseTime(ns): " <<firstPulseTime.GetSeconds()*1e9<<endl;

    while(firstPulseTime < WaveSimFirstTime){
        BufferSvc->pop_PulseBufferFront();       
        firstPulseTime = BufferSvc->get_firstPulseTime();
        //LogInfo<<"in while, firstPulseTime(ns): " <<firstPulseTime.GetSeconds()*1e9<<endl;

    }


    //get pulse_vector for WaveFormSim
    
    pulse_vector = BufferSvc->get_PulseVector(WaveSimLastTime);


}







