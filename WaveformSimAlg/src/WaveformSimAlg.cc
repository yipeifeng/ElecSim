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

////////////////

    declProp("PulseSampleWith", m_PulseSampleWith=150);  //unit ns, the time window for one Ideal pulse waveform with
    declProp("enableOvershoot", m_enableOvershoot=false);
    declProp("enableSatuation", m_enableSatuation=false);
    declProp("enableNoise", m_enableNoise=false);
    declProp("simFrequency", m_simFrequency=1e9);
    declProp("noiseAmp", m_noiseAmp=0.5e-3);
    declProp("speAmp", m_speAmp=5.6e-3);
    declProp("PmtTotal", m_PmtTotal=17746);
    declProp("waveform_width", m_width=14e-9);
    declProp("waveform_mu", m_mu=0.45);

    m_linearityThreshold = 20;





}

WaveformSimAlg::~WaveformSimAlg(){

}





bool WaveformSimAlg::initialize(){

    get_Services();
    loadResponse();

    return true;

}



bool WaveformSimAlg::execute(){

    LogInfo<<"execute WaveformSimAlg! "<<endl;
    clear_vector();//clear temp_pulse_vector

    load_Pulse();

    produce_Waveform();






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



void WaveformSimAlg::produce_Waveform(){

    LogInfo<<"produce_Waveform!! to do" <<endl;
    
    // Organize Pulses by Channel(pmtID)
    map<int, vector<Pulse> > pulseMap;
    mapPulsesByChannel(pulse_vector, pulseMap);


    for(int channelId=0; channelId<m_PmtTotal; channelId++){

        if(pulseMap[channelId].size() > 0){
            generateOneChannel(channelId, pulseMap[channelId]); 
        }



    }



}



void WaveformSimAlg::loadResponse(){
    double dT_seconds = (1. / m_simFrequency);
    double PulseSampleWith = m_PulseSampleWith*1e-9; //unit s

    int nPulseSamples = int(PulseSampleWith/dT_seconds);
    int nOvershootSamples = int(16000/(dT_seconds*1e9)); //1 second == 1e9 ns
    m_pmtPulse.resize(nPulseSamples);
    m_overshoot.resize(nOvershootSamples);


    for (int i=0; i<nPulseSamples; i++) {
        m_pmtPulse[i] = pmtPulse(i*dT_seconds,1);
    }


    for (int i=0; i<nOvershootSamples; i++) {

        m_overshoot[i] = 0;
        // Store overshoot for raw signal
        if(m_enableOvershoot){
            m_overshoot[i] += overshoot(i*dT_seconds);
        }
    }


    //////dump information


    //            for(int i=0; i<nPulseSamples; i++){
    //                LogInfo<<"Ideal Pulse Waveform Sample: " << m_pmtPulse[i]<<"  "<< i<<endl;
    //            }

    if( !m_enableOvershoot ){
        LogInfo<<"Overshoot disabled"<<endl; 
    }else{
        LogInfo<<"Overshoot enabled"<<endl;
    }


    if( !m_enableSatuation ){ 
        LogInfo << "Saturation disabled" << endl; 
    }else{ 
        LogInfo << "Saturation enabled" << endl;
    }


}




double WaveformSimAlg::pmtPulse(double deltaT, int nPulse) {
    // Return ideal single pe pulse with amplitude 1 V
    double width; // pulse width parameter
    double mu; // parameter determining the degree of asymmetry
    //if ( nPulse > 1 ){
    //    width = getPulseWidth( nPulse); 
    //    mu    = getPulseForm( nPulse);
    //}
    //else {
    //    width = 7.5e-9;
    //    mu    = 0.45;
    //}

    //width = 14e-9;   //fangxiao change it to 20inch MCP-PMT's parameter
    //mu = 0.45;

    width = m_width;
    mu = m_mu;

    double shift = 6e-9 -width/1.5;
    if (deltaT-shift<0) return 0.;


    return - exp( -pow( log( (deltaT-shift)/width),2) 
            / ( 2*pow(mu,2) ) ) ;  // unit V
}



double WaveformSimAlg::overshoot(double deltaT) {
    if (deltaT < 0) return 0.;
    double amp = 0.045; // Relative overshoot amplitude for spe pulses
    // Fermi onset
    double t0   = 50e-9; 
    double t1   = 10e-9;
    double fermi = 1. / (exp( (t0 - deltaT) / t1) + 1.);
    // Exponential overshoot component
    double tau = 145.e-9; // Overshoot decay time in s
    double expoOS = exp(-(deltaT-87e-9)/tau);
    // Slower overshoot component
    double mean = 0.4e-6;
    double sigma = 0.08e-6;
    double t = deltaT -mean;
    double gausOS = 0.12 * exp(pow(t,2)/(-2*pow(sigma,2)));
    // Undershoot 
    mean = 0.65e-6;
    sigma = 0.12e-6;
    t = deltaT -mean;
    double undershoot = -0.03 * exp(pow(t,2)/(-2*pow(sigma,2)));

    return  amp * fermi * (expoOS + gausOS + undershoot)
        ; //unit V
}


void WaveformSimAlg::mapPulsesByChannel(vector<Pulse>& pulse_vector, 
        map<int, std::vector<Pulse> >& pulseMap){


    for (vector<Pulse>::iterator it=pulse_vector.begin(); 
            it != pulse_vector.end(); ++it) {

        (pulseMap[it->pmtID()]).push_back(*it); 
    }
}



void WaveformSimAlg::generateOneChannel(int channelId, vector<Pulse>& channelPulses){

    //LogInfo<<"channelId: " << channelId <<"  Pulse Num: " << channelPulses.size()<<endl;


}






