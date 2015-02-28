#include "PMTSimAlg.h"
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
#include <iterator>

using namespace std;

DECLARE_ALGORITHM(PMTSimAlg);

PMTSimAlg::PMTSimAlg(const string& name):AlgBase(name){

    declProp("HitBufferLength",m_HitBufferLength=2000);
    declProp("HitVectorLength",m_HitVectorLength=500);



    declProp("PmtTotal",m_PmtTotal=17746);
    declProp("enableAfterPulse", m_enableAfterPulse=true);
    declProp("enableDarkPulse", m_enableDarkPulse=true);
    declProp("enableEfficiency", m_enableEfficiency=false);
    declProp("enableAssignGain", m_enableAssignGain=false);
    declProp("enableAssignSigmaGain", m_enableAssignSigmaGain=false);
    declProp("inputGain", m_Gain=1);
    declProp("inputSigmaGain", m_SigmaGain=0.3);



    declProp("preTimeTolerance", m_preTimeTolerance=50);
    declProp("postTimeTolerance", m_postTimeTolerance=100);
    declProp("expWeight", m_expWeight=0.01);
    declProp("speExpDecay", m_speExpDecay=1.1);
    declProp("darkRate", m_darkRate=50e3);

}

PMTSimAlg::~PMTSimAlg(){

}





bool PMTSimAlg::initialize(){
    get_Services();

    return true;
}



bool PMTSimAlg::execute(){
    LogInfo<<"execute PMTSim" <<endl;

    clear_vector();
    load_Hit();
    produce_Pulse();

    return true;
}


bool PMTSimAlg::finalize(){

    return true;
}



void PMTSimAlg::clear_vector(){
    hit_vector.clear();

}


void PMTSimAlg::load_Hit(){

    LogInfo<<"load_Hit!" <<endl;

////////If Hit buffer don't have enough Hit, I incdent::fire UnpackingTask to get more Hits.

    int m_HitBufferSize = BufferSvc->get_HitBufferSize();

    TimeStamp delta_HitTimeStamp(0);

    if(m_HitBufferSize >= 2){
        TimeStamp firstHitTime = BufferSvc->get_firstHitTime();

        TimeStamp lastHitTime = BufferSvc->get_lastHitTime();

        delta_HitTimeStamp = lastHitTime - firstHitTime;
    }


    while(delta_HitTimeStamp.GetSeconds() * 1e9 < m_HitBufferLength){
        LogInfo<<"HitBuffer don't have enough Hit , Incident::fire UnpackingTask"<<endl;

        Incident::fire("UnpackingTask");

        m_HitBufferSize = BufferSvc->get_HitBufferSize();

        LogInfo<<"HitBuffer size: "<<m_HitBufferSize<<endl;

        if(m_HitBufferSize >= 2){
            TimeStamp firstHitTime = BufferSvc->get_firstHitTime();

            TimeStamp lastHitTime = BufferSvc->get_lastHitTime();

            delta_HitTimeStamp = lastHitTime - firstHitTime;
            LogInfo<<"delta_HitTimeStamp: " << delta_HitTimeStamp.GetSeconds()*1e9<<endl;
        }
    }

    ///////// get hit_vector to convert to pulse



    hit_vector = BufferSvc -> get_HitVector(m_HitVectorLength);
    LogInfo<<"temp hit_vector size: " << hit_vector.size()<<endl;



}


void PMTSimAlg::get_Services(){


    SniperPtr<IGlobalTimeSvc> TimeSvcPtr(Task::top(),"GlobalTimeSvc"); 
    TimeSvc = TimeSvcPtr.data();


    SniperPtr<IElecBufferMgrSvc> BufferSvcPtr(Task::top(),"ElecBufferMgrSvc");
    BufferSvc = BufferSvcPtr.data();


    SniperPtr<IPmtParamSvc> PmtDataSvcPtr(Task::top(),"PmtParamSvc");
    PmtDataSvc = PmtDataSvcPtr.data();


}







void PMTSimAlg::produce_Pulse(){

//loop hit_vector and convert it to pulse

    vector<Hit>::iterator hvIter; 
    vector<Hit>::iterator hvDone = hit_vector.end();


    for (hvIter=hit_vector.begin(); hvIter != hvDone; ++hvIter) {

        TimeStamp m_hitTime = hvIter->hitTime() ;  
        double m_weight = hvIter->weight();//The weight of the hit
        int m_pmtID = hvIter->pmtID(); 

        double m_efficiency = PmtDataSvc->get_efficiency(m_pmtID)  ; //pmtData the PMT efficiency to determine whether ignore SimHit


        double m_gain=0;
        double m_sigmaGain=0;

        if(m_enableAssignGain){
            m_gain = m_Gain;
        }else{
            m_gain = PmtDataSvc->get_gain(m_pmtID);
        }

        if(m_enableAssignSigmaGain){
            m_sigmaGain = m_SigmaGain;
        }else{
            m_sigmaGain = PmtDataSvc->get_sigmaGain(m_pmtID);//pmtData sigmaGain
        }

        double m_afterPulseProb = PmtDataSvc->get_afterPulseProb(m_pmtID);
        double m_timeSpread = PmtDataSvc->get_timeSpread(m_pmtID);
        double m_timeOffset = PmtDataSvc->get_timeOffset(m_pmtID);


        if(m_enableEfficiency){ 
            if(gRandom->Rndm() > m_efficiency){
                continue;    //ignore SimHit due to efficiency
            }
        }





    }






























    int pmtID_1=0;
    int pmtID_2=1;

    double amp_1 = 10;
    double amp_2 = 15;

    TimeStamp time_1(0); //second
    TimeStamp time_2(1); //second

    TimeStamp evt_time_1(0); //second
    TimeStamp evt_time_2(1); //second

    Pulse pulse_1(pmtID_1, amp_1, time_1, evt_time_1);
    Pulse pulse_2(pmtID_2, amp_2, time_2, evt_time_2);

    BufferSvc->save_to_PulseBuffer(pulse_1);
    BufferSvc->save_to_PulseBuffer(pulse_2);

    LogInfo<<"PulseBuffer size: " << BufferSvc->get_PulseBuffer().size()<<endl;

}








