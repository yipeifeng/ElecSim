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

    declProp("HitBufferLength",m_HitBufferLength=2000);//unit ns
    declProp("HitVectorLength",m_HitVectorLength=500); //unit ns



    declProp("PmtTotal",m_PmtTotal=17746);
    declProp("enableAfterPulse", m_enableAfterPulse=false);
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


    m_afterPulseAmpMode ="SinglePE"; // Mode for afterpulse amplitude distribution
    m_darkPulseAmpMode ="SinglePE"; // Mode for dark pulse amplitude distribution 


    m_afterPulsePdf.clear();
    m_afterPulseEdges.clear();

    for(int ii=0; ii < NUM_BINS_DIST; ii++) {
        m_afterPulsePdf.push_back(afterPusleTimingDist[ii]);
        m_afterPulseEdges.push_back(ii*100.+500.);
    }

    getAfterPulseAmpPdf(m_afterPulseAmpPdf); 
    getAfterPulseAmpEdges(m_afterPulseAmpEdges);

    TimeStamp tempTime(0);
    startTime = tempTime;
    endTime = tempTime;
    startEvtTimeStamp = tempTime;
    deltaSimTime = 0;


}

PMTSimAlg::~PMTSimAlg(){

}





bool PMTSimAlg::initialize(){
    get_Services();

    return true;
}



bool PMTSimAlg::execute(){
    LogInfo<<"execute PMTSim" <<endl;

    clear_vector(); // clear temp hit_vector
    load_Hit();
    produce_Pulse();
    sort_PulseBuffer();


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

    ////////If Hit buffer doesn't have enough Hit, I incdent::fire UnpackingTask to get more Hits.

    int m_HitBufferSize = BufferSvc->get_HitBufferSize();

    TimeStamp delta_HitTimeStamp(0);

    if(m_HitBufferSize >= 2){
        TimeStamp firstHitTime = BufferSvc->get_firstHitTime();

        TimeStamp lastHitTime = BufferSvc->get_lastHitTime();

        delta_HitTimeStamp = lastHitTime - firstHitTime;
    }

    LogInfo<<"delta_HitTimeStamp(ns): " << delta_HitTimeStamp.GetSeconds() * 1e9<<endl;


    while(delta_HitTimeStamp.GetSeconds() * 1e9 < m_HitBufferLength){
        LogInfo<<"HitBuffer doesn't have enough Hit , Incident::fire UnpackingTask"<<endl;

        Incident::fire("UnpackingTask");

        m_HitBufferSize = BufferSvc->get_HitBufferSize();

        LogInfo<<"HitBuffer size: "<<m_HitBufferSize<<endl;

        if(m_HitBufferSize >= 2){
            TimeStamp firstHitTime = BufferSvc->get_firstHitTime();

            TimeStamp lastHitTime = BufferSvc->get_lastHitTime();

            delta_HitTimeStamp = lastHitTime - firstHitTime;
            LogInfo<<"delta_HitTimeStamp(ns): " << delta_HitTimeStamp.GetSeconds()*1e9<<endl;
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

    startTime = hit_vector.front().hitTime();
    startEvtTimeStamp = hit_vector.front().EvtTimeStamp();
    endTime =hit_vector.back().hitTime();

    TimeStamp delta_Time = endTime - startTime;

    deltaSimTime = delta_Time.GetSeconds();//unit s (convert ns to s)
    //LogInfo<<"startTime: " <<startTime<<endl;
    LogInfo<<"deltaSimTime(ns): " << deltaSimTime*1e9<<endl;

    for (hvIter=hit_vector.begin(); hvIter != hvDone; ++hvIter) {

        TimeStamp m_hitTime = hvIter->hitTime() ;  
        //LogInfo<<"m_hitTime: " << m_hitTime<<endl;
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

        double transitTime = gRandom->Gaus(0,1) * m_timeSpread + m_timeOffset;

        TimeStamp pulseHitTime = m_hitTime;
        pulseHitTime.Add(transitTime*1e-9);

        //LogInfo<<"pulseHitTime: " <<pulseHitTime<<endl;

        TimeStamp EvtTimeStamp = hvIter->EvtTimeStamp();

        double amplitude = PulseAmp(m_weight,m_gain,m_sigmaGain);

        Pulse pulse(m_pmtID, amplitude, pulseHitTime, EvtTimeStamp );

        BufferSvc->save_to_PulseBuffer(pulse);

        //add afterPulse

        if(m_enableAfterPulse){

            if (gRandom->Rndm() < m_afterPulseProb){ 
                BufferSvc->save_to_PulseBuffer( makeAfterPulse(pulse ) );
            }

        }


    }


    // Process dark pulses
    if(m_enableDarkPulse){
        // Get list of all detector sensors

        for ( int pmtID=0; pmtID < m_PmtTotal; ++pmtID) {

            // Generate Poisson-distributed number around mean number of dark hits in simulation time window
            //      int Ndark = PoissonRand( (pd_vector[pmtID].darkRate()+(m_darkRate - 15e3) ) 
            int Ndark = PoissonRand( 
                    (PmtDataSvc->get_darkRate(pmtID)+(m_darkRate - 15e3) ) 
                    *deltaSimTime
                    ); //the mean value of old darkRate is 15e3,so we substract 15e3 for assign it from Property, the deltaSimTime unit is s.

            for (int dummy = 0; dummy < Ndark; ++dummy) {
                BufferSvc->save_to_PulseBuffer(makeDarkPulse(pmtID) );

            }
        }
    }

    LogInfo<<"PulseBuffer size: " << BufferSvc->get_PulseBuffer().size()<<endl;

}





double PMTSimAlg::PulseAmp(double weight,double gain, double sigmaGain){

    double amp;
    //    double m_expWeight = 0.01; //Weight of the exponential contribution to the spe response function
    //    double m_speExpDecay = 1.1;//Decay time of the exponential contribution to the spe response function
    double m_speCutoff = 0.15; //Charge cut against which the PMT efficiency is computed

    //include relative gain
    double randW = gRandom->Rndm();
    if (randW > m_expWeight || weight >1.1){
        amp = gRandom->Gaus(0,1) * sigmaGain * TMath::Sqrt(weight) + gain * weight;
    }
    else {
        amp = (gRandom->Exp(m_speExpDecay) + m_speCutoff) * gain * weight;
    }
    if(amp<0) amp = 0;

    return amp;
}


Pulse PMTSimAlg::makeAfterPulse(Pulse pulse){

    // Time offset from main pulse based on time PDF of after-pulses
    // double current_rand = m_randAfterPulseTime();

    TimeStamp afterPulseTime = pulse.pulseHitTime();
    TimeStamp EvtTimeStamp = pulse.EvtTimeStamp();

    int m_pmtID = pulse.pmtID();
    double amplitude;

    double current_rand = gRandom -> Rndm();
    double delta_afterPulseTime = ConvertPdfRand01(current_rand,m_afterPulsePdf,m_afterPulseEdges) ; 

    //LogInfo<<"delta_afterPulseTime: " <<delta_afterPulseTime<<endl;

    if(delta_afterPulseTime>10000){
        delta_afterPulseTime = 10000; 
    }

    afterPulseTime.Add(delta_afterPulseTime * 1e-9);  //convert ns to s.


    if(m_afterPulseAmpMode == "SinglePE") { 

        if(m_enableAssignGain && m_enableAssignSigmaGain){
            amplitude = PulseAmp(pulse.amplitude(), m_Gain, m_SigmaGain);
        }else{
            amplitude = PulseAmp(pulse.amplitude(),PmtDataSvc->get_gain(m_pmtID), PmtDataSvc->get_sigmaGain(m_pmtID) );

        }
    }


    if(m_afterPulseAmpMode == "PDF"){
        current_rand = gRandom -> Rndm();
        amplitude = ConvertPdfRand01(current_rand,m_afterPulseAmpPdf,m_afterPulseAmpEdges);
        amplitude = PulseAmp(amplitude,PmtDataSvc->get_gain(m_pmtID), PmtDataSvc->get_sigmaGain(m_pmtID) );
    }

    Pulse afterpulse(m_pmtID, amplitude, afterPulseTime, EvtTimeStamp);

    return afterpulse;
}




double PMTSimAlg::ConvertPdfRand01 (double rand,vector<double> pdf, vector<double> edges){
    // Defined PDF returns random number in [0,1] distributed according to user-defined histogram.
    // It assumes even bin sizes, so accomodate uneven bin sizes for generality.
    int current_bin;
    int Nbins = pdf.size();

    for(int bin=0; bin<Nbins; bin++) {
        if(rand >= pdf[bin] && rand < pdf[bin+1]) {
            current_bin = bin;
            break;
        }
        else
            current_bin = Nbins-1;
    }


    return edges[current_bin] + (rand-pdf[current_bin])*(edges[current_bin+1]-edges[current_bin])
        /(pdf[current_bin+1]-pdf[current_bin]);

}



Pulse PMTSimAlg::makeDarkPulse(int pmtID) {
    double amplitude = 0.0;
    TimeStamp pulseHitTime = startTime;
    TimeStamp EvtTimeStamp = startEvtTimeStamp;

    double darkPulseTime = gRandom->Rndm() * deltaSimTime;
    pulseHitTime.Add(darkPulseTime*1e-9) ; //unit s

    if(m_darkPulseAmpMode == "SinglePE"){

        if(m_enableAssignGain && m_enableAssignSigmaGain){
            amplitude = PulseAmp(1.0, m_Gain, m_SigmaGain);
        }
        else {
            amplitude = PulseAmp(1.0, PmtDataSvc->get_gain(pmtID), PmtDataSvc->get_sigmaGain(pmtID) );
        }

    }

    if(m_darkPulseAmpMode == "PDF"){
        double current_rand = gRandom->Rndm();
        amplitude = ConvertPdfRand01(current_rand,m_afterPulseAmpPdf, m_afterPulseAmpEdges);
        amplitude = PulseAmp(amplitude, PmtDataSvc->get_gain(pmtID), PmtDataSvc->get_sigmaGain(pmtID) );
    }
    Pulse darkpulse(pmtID, amplitude, pulseHitTime, EvtTimeStamp);

    return darkpulse;
}




int PMTSimAlg::PoissonRand(double mean) {
    // Using source code from ROOT's TRandom::Poisson
    // Note: ROOT uses different algorithms depending on the mean, but mean is small 
    //       for our purposes, so use algorithm for mean<25

    int n;
    if (mean <= 0) return 0;

    double expmean = exp(-mean);
    double pir = 1;
    n = -1;
    while(1) {
        n++;
        pir *= gRandom->Rndm();
        if (pir <= expmean) break;
    }
    return n;
}




void PMTSimAlg::getAfterPulseAmpPdf(vector<double>& pdf){
    pdf.push_back(0);pdf.push_back(0.0219574);pdf.push_back(0.0931247);pdf.push_back(0.179757);
    pdf.push_back(0.264803);pdf.push_back(0.342568);pdf.push_back(0.411712);pdf.push_back(0.472498);
    pdf.push_back(0.525729);pdf.push_back(0.572335);pdf.push_back(0.613205);pdf.push_back(0.649137);
    pdf.push_back(0.702162);pdf.push_back(0.745131);pdf.push_back(0.780295);pdf.push_back(0.809341);
    pdf.push_back(0.842677);pdf.push_back(0.868769);pdf.push_back(0.889482);pdf.push_back(0.906131);
    pdf.push_back(0.930777);pdf.push_back(0.947671);pdf.push_back(0.95962);pdf.push_back(0.968294);
    pdf.push_back(0.974731);pdf.push_back(0.983341);pdf.push_back(0.988564);pdf.push_back(0.99189);
    pdf.push_back(0.994094);pdf.push_back(0.995601);pdf.push_back(0.996661);pdf.push_back(0.997423);
    pdf.push_back(0.997983);pdf.push_back(0.998401);pdf.push_back(0.998717);pdf.push_back(0.999151);
    pdf.push_back(0.999418);pdf.push_back(0.999591);pdf.push_back(0.999705);pdf.push_back(0.999783);
    pdf.push_back(0.999838);pdf.push_back(0.999876);pdf.push_back(0.999905);pdf.push_back(0.999926);
    pdf.push_back(0.999941);pdf.push_back(0.999953);pdf.push_back(0.999962);pdf.push_back(0.999969);
    pdf.push_back(0.999975);pdf.push_back(0.999979);pdf.push_back(0.999983);pdf.push_back(1.0);
}


void PMTSimAlg::getAfterPulseAmpEdges(vector<double>& edges){
    edges.push_back(0.5);edges.push_back(0.7);edges.push_back(0.9);edges.push_back(1.1);
    edges.push_back(1.3);edges.push_back(1.5);edges.push_back(1.7);edges.push_back(1.9);
    edges.push_back(2.1);edges.push_back(2.3);edges.push_back(2.5);edges.push_back(2.7);
    edges.push_back(3.05);edges.push_back(3.4);edges.push_back(3.75);edges.push_back(4.1);
    edges.push_back(4.6);edges.push_back(5.1);edges.push_back(5.6);edges.push_back(6.1);
    edges.push_back(7.1);edges.push_back(8.1);edges.push_back(9.1);edges.push_back(10.1);
    edges.push_back(11.1);edges.push_back(13.1);edges.push_back(15.1);edges.push_back(17.1);
    edges.push_back(19.1);edges.push_back(21.1);edges.push_back(23.1);edges.push_back(25.1);
    edges.push_back(27.1);edges.push_back(29.1);edges.push_back(31.1);edges.push_back(35.1);
    edges.push_back(39.1);edges.push_back(43.1);edges.push_back(47.1);edges.push_back(51.1);
    edges.push_back(55.1);edges.push_back(59.1);edges.push_back(63.1);edges.push_back(67.1);
    edges.push_back(71.1);edges.push_back(75.1);edges.push_back(79.1);edges.push_back(83.1);
    edges.push_back(87.1);edges.push_back(91.1);edges.push_back(95.1);edges.push_back(100);
}



void PMTSimAlg::sort_PulseBuffer(){

    BufferSvc->SortPulseBuffer();

}








