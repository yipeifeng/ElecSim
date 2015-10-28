#include "PreTrgAlg.h"
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
#include "ElecDataStruct/Pulse.h"
#include <deque>


using namespace std;

DECLARE_ALGORITHM(PreTrgAlg);


PreTrgAlg::PreTrgAlg(const string& name):AlgBase(name){
    declProp("PulseBufferLength",m_PulseBufferLength=2000);//unit ns

}


PreTrgAlg::~PreTrgAlg(){

}


bool PreTrgAlg::initialize(){

    get_Services();

    return true;
}


bool PreTrgAlg::execute(){

    LogInfo<<"execute PreTrgAlg !"<<endl;

    find_Trg_from_PulseBuffer(); // for sample test,I get trigger from PulseBuffer.If the hits in buffer not enough, I do the UnpackingAlg.







    return true;
}



bool PreTrgAlg::finalize(){


    return true;
}



bool PreTrgAlg::find_Trg_from_PulseBuffer(){

    //when I find Trg, first I make sure there are enough Pulse in the Buffer .



    int m_PulseBufferSize = BufferSvc->get_PulseBufferSize();

    //LogInfo<<"m_PulseBufferSize: " <<m_PulseBufferSize<<endl;
    TimeStamp delta_PulseTimeStamp(0);   //I set the initial delta_PulseTimeStamp = 0, if the m_PulseBufferSize < 2, the delta_PulseTimeStamp = 0.

    if(m_PulseBufferSize >= 2){
        TimeStamp firstPulseTime = BufferSvc->get_firstPulseTime();

        TimeStamp lastPulseTime = BufferSvc->get_lastPulseTime();


        delta_PulseTimeStamp = lastPulseTime - firstPulseTime;
    }

    LogInfo<<"delta_PulseTimeStamp(ns): " << delta_PulseTimeStamp.GetSeconds()*1e9<<endl;

    while(delta_PulseTimeStamp.GetSeconds() * 1e9 < m_PulseBufferLength){
        LogInfo<<"PulseBuffer don't have enough Pulse , Incident::fire PmtSimTask"<<endl;

        Incident::fire("PMTSimTask");

        m_PulseBufferSize = BufferSvc->get_PulseBufferSize();

        LogInfo<<"PulseBuffer size: "<<m_PulseBufferSize<<endl;

        if(m_PulseBufferSize >= 2){
            TimeStamp firstPulseTime = BufferSvc->get_firstPulseTime();

            LogInfo<<"firstPulseTime(ns): " <<firstPulseTime.GetSeconds()*1e9<<endl;

            TimeStamp lastPulseTime = BufferSvc->get_lastPulseTime();

            LogInfo<<"lastPulseTime(ns): "<<lastPulseTime.GetSeconds()*1e9<<endl;

            delta_PulseTimeStamp = lastPulseTime - firstPulseTime;
            LogInfo<<"delta_PulseTimeStamp(ns): " << delta_PulseTimeStamp.GetSeconds()*1e9<<endl;
        }
    }

    //get Trigger TimeStamp

    TimeStamp TriggerTime = BufferSvc->get_firstPulseTime(); //for sample test, use firstPulseTime as TriggerTime

    BufferSvc->save_to_TriggerBuffer(TriggerTime);




    return true;

}


bool PreTrgAlg::get_Services(){

    SniperPtr<IGlobalTimeSvc> TimeSvcPtr(Task::top(),"GlobalTimeSvc"); 
    TimeSvc = TimeSvcPtr.data();


    SniperPtr<IElecBufferMgrSvc> BufferSvcPtr(Task::top(),"ElecBufferMgrSvc");
    BufferSvc = BufferSvcPtr.data();



    return true;
}






















