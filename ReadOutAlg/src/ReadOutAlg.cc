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
    get_Services();

    return true;
}


bool ReadOutAlg::execute(){

    LogInfo<<"begin event: " <<m_evtID<<endl; 
    get_TriggerTime();

    CheckOutWaveform();

    pop_TriggerTime();//every evt, use one TriggerTime and pop it 




    m_evtID++;
    return true;
}


bool ReadOutAlg::finalize(){


    return true;
}



bool ReadOutAlg::get_Services(){

    SniperPtr<IGlobalTimeSvc> TimeSvcPtr(Task::top(),"GlobalTimeSvc"); 
    TimeSvc = TimeSvcPtr.data();


    SniperPtr<IElecBufferMgrSvc> BufferSvcPtr(Task::top(),"ElecBufferMgrSvc");
    BufferSvc = BufferSvcPtr.data();



    return true;
}


bool ReadOutAlg::put_hit_into_buffer(){

    TTimeStamp Evt_TimeStamp = TimeSvc->get_current_evt_time();
    TimeStamp m_Evt_TimeStamp(Evt_TimeStamp.GetSec(),Evt_TimeStamp.GetNanoSec());//convert TTimeStamp to TimeStamp


    TimeStamp firstHitTime(0); //create variable to save firstHitTime


    //find firstHitTime , if no hit in buffer, the firstHitTime=start_time
    if(BufferSvc->get_HitBuffer().size() == 0){
        firstHitTime = TimeSvc->get_start_time(); //this function return TimeStamp type
        LogInfo<<"the buffer is empty, so the firstHitTime is start time: "<<firstHitTime<<endl;

    }else{
        firstHitTime = BufferSvc->get_firstHitTime();
        LogInfo<<"firstHitTime in buffer: " <<firstHitTime<<endl;
    }


    LogInfo<<"current Evt_TimeStamp: " <<m_Evt_TimeStamp<<endl;

    //to control the HitBuffer length 
    TimeStamp delta_TimeStamp = m_Evt_TimeStamp - firstHitTime ;
    LogInfo<<"delta_TimeStamp: " << delta_TimeStamp<<endl;
    LogInfo<<"delta_TimeStamp in nanosecond: " << delta_TimeStamp.GetSeconds() * 1e9<<endl;



    while(delta_TimeStamp.GetSeconds() * 1e9 < 2000){

        LogInfo<<"Incident::fire UnpackingTask"<<endl;
        Incident::fire("UnpackingTask");


        Evt_TimeStamp = TimeSvc->get_current_evt_time();
        TimeStamp m_Evt_TimeStamp(Evt_TimeStamp.GetSec(),Evt_TimeStamp.GetNanoSec());
        LogInfo<<"current Evt_TimeStamp: " <<m_Evt_TimeStamp<<endl;


        firstHitTime = BufferSvc->get_firstHitTime();
        LogInfo<<"firstHitTime in buffer: " <<firstHitTime<<endl;

        delta_TimeStamp = m_Evt_TimeStamp - firstHitTime ;
        LogInfo<<"delta_TimeStamp: " << delta_TimeStamp<<endl;
        LogInfo<<"delta_TimeStamp in nanosecond: " << delta_TimeStamp.GetSeconds() * 1e9<<endl;

    }

    return true;
}



bool ReadOutAlg::get_TriggerTime(){

    m_TriggerBuffer = BufferSvc->get_TriggerBuffer();

    LogInfo<<"Trigger Buffer size: " << m_TriggerBuffer.size()<<endl;

    if(m_TriggerBuffer.size() == 0){
        LogInfo<<"Trigger Buffer size is 0, do PreTrgTask" <<endl;

        LogInfo<<"Incident::fire PreTrgTask"<<endl;
        Incident::fire("PreTrgTask");

    }
    
    
   TriggerTime = BufferSvc->get_TriggerTimeStamp();  

   LogInfo<<"TriggerTime: "  << TriggerTime.GetSeconds()*1e9<<endl;


    return true;
}



void ReadOutAlg::CheckOutWaveform(){

   // bool Waveform_is_enough = BufferSvc->WaveformBufferEnough(TriggerTime, ReadOutLength);


    bool Waveform_is_enough = false;

    if(Waveform_is_enough == false){
    
    
        LogInfo<<"Waveform isn't enough, Incident::fire WaveformSimTask"<<endl;
        Incident::fire("WaveformSimTask");
    
    }


}



void ReadOutAlg::pop_TriggerTime(){
    BufferSvc->pop_TriggerTimeStamp();
}














