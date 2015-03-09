#include "PmtParamSvc.h"
#include "SniperKernel/SvcFactory.h"
#include "SniperKernel/SniperLog.h"
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>


using namespace std;


//class PmtData


int PmtData::pmtId(){
    return m_pmtId; 
}

double PmtData::efficiency(){
    return m_efficiency; 
}

double PmtData::gain(){
    return m_gain; 
}

double PmtData::sigmaGain(){
    return m_sigmaGain; 
}

double PmtData::afterPulseProb(){
    return m_afterPulseProb;
}

double PmtData::prePulseProb(){
    return m_prePulseProb; 
}

double PmtData::darkRate(){
    return m_darkRate; 
}

double PmtData::timeSpread(){
    return m_timeSpread;
}

double PmtData::timeOffset(){
    return m_timeOffset; 
}




DECLARE_SERVICE(PmtParamSvc);


PmtParamSvc::PmtParamSvc(const std::string& name)
    : SvcBase(name)
{
    declProp("PmtDataFile", m_pmtdata_file="PmtData.root");
    declProp("PmtTotal", m_PmtTotal=17746);
}


PmtParamSvc::~PmtParamSvc(){

}


bool PmtParamSvc::initialize(){

    LogInfo<<"init PmtParamSvc"<<endl;

    pd_vector.clear();


    int m_pmtId;
    double m_efficiency;
    double m_gain;
    double m_sigmaGain;
    double m_afterPulseProb;
    double m_prePulseProb; 
    double m_darkRate;
    double m_timeSpread;
    double m_timeOffset;

    TFile* f1 = new TFile(m_pmtdata_file.c_str()); 
    TTree* t1 = (TTree*) f1->Get("PmtData");
    t1 ->SetBranchAddress("pmtId",&m_pmtId);
    t1 ->SetBranchAddress("efficiency",&m_efficiency);
    t1 ->SetBranchAddress("gain",&m_gain);
    t1 ->SetBranchAddress("sigmaGain",&m_sigmaGain);
    t1 ->SetBranchAddress("afterPulseProb",&m_afterPulseProb);
    t1 ->SetBranchAddress("prePulseProb",&m_prePulseProb);
    t1 ->SetBranchAddress("darkRate",&m_darkRate);
    t1 ->SetBranchAddress("timeSpread",&m_timeSpread);
    t1 ->SetBranchAddress("timeOffset",&m_timeOffset); 

    for(int i=0; i<m_PmtTotal; i++){
        t1->GetEntry(i);
        PmtData pd(m_pmtId,
                m_efficiency,
                m_gain,
                m_sigmaGain,
                m_afterPulseProb,
                m_prePulseProb,
                m_darkRate,
                m_timeSpread,
                m_timeOffset);

        pd_vector.push_back(pd);
 //       LogInfo<<"m_efficiency: " << m_efficiency<<endl;
    }
    return true;
}


bool PmtParamSvc::finalize(){

    return true;
}


double PmtParamSvc::get_efficiency(int pmtId){
    return pd_vector[pmtId].efficiency();
}


double PmtParamSvc::get_gain(int pmtId){
    return pd_vector[pmtId].gain();
}


double PmtParamSvc::get_sigmaGain(int pmtId){
    return pd_vector[pmtId].sigmaGain();
}


double PmtParamSvc::get_afterPulseProb(int pmtId){
    return pd_vector[pmtId].afterPulseProb();
}

double PmtParamSvc::get_timeSpread(int pmtId){
    return pd_vector[pmtId].timeSpread();
}

double PmtParamSvc::get_timeOffset(int pmtId){
    return pd_vector[pmtId].timeOffset();
}


double PmtParamSvc::get_darkRate(int pmtId){
    return pd_vector[pmtId].darkRate();
}





