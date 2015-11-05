#include "EvtMixingAlg.h"
#include "SniperKernel/AlgFactory.h"
#include "BufferMemMgr/IDataMemMgr.h"
#include "DataRegistritionSvc/DataRegistritionSvc.h"
#include "SniperKernel/Incident.h"
#include "InputReviser/InputReviser.h"
#include "Identifier/CdID.h"
#include "Event/SimHeader.h"
#include "Event/SimEvent.h"
#include "Event/SimPMTHit.h"
#include "Event/ElecHeader.h"
#include "Event/ElecEvent.h"
#include "Event/ElecFeeCrate.h"
#include "Event/ElecFeeChannel.h"
#include <time.h>
#include <TMath.h>
#include <TRandom.h>
#include <TStopwatch.h>
#include <TTimeStamp.h>
#include "Context/TimeStamp.h"
#include "SniperKernel/Task.h"
#include "GlobalTimeSvc/IGlobalTimeSvc.h"


using namespace std;

DECLARE_ALGORITHM(EvtMixingAlg);

EvtMixingAlg::EvtMixingAlg(const string& name):AlgBase(name)
{
    declProp("RateMap", m_rateMap);
    declProp("TaskMap", m_taskMap);

}



EvtMixingAlg::~EvtMixingAlg(){}


bool EvtMixingAlg::initialize(){


    m_totalRate = 0;
    for(map<string,double>::iterator it=m_rateMap.begin();it!=m_rateMap.end();it++){ //key is sample name ,value is the rate of sample
        m_totalRate+=it->second;  //first is sample name , second is rate 
        m_firstMap.insert(std::pair<string,bool>(it->first,true));// I define this map just for find the begin entry of the sample
    } 
    LogInfo<<"event totalRate= "<< m_totalRate << endl;
    m_mainTau=1.0/m_totalRate;
    LogInfo<< "mainTau= " << m_mainTau <<endl;

    m_memMgr_top = SniperPtr<IDataMemMgr>(getScope(),"BufferMemMgr").data();  

    for(map<string,string>::iterator it=m_taskMap.begin();it!=m_taskMap.end();it++){
        InputReviser* aInci = new InputReviser(it->second, true);  //first is task name 
        m_incidentMap.insert(make_pair(it->first, aInci));//first is sample name，second is the InputReviser which related to the sample name. In test I use sample as task name.
    }

    LogInfo<<"____initialize success.____"<<endl;

    return true;
}



bool EvtMixingAlg::execute(){

    JM::EvtNavigator* Nav = get_one_event();
    double delta = gRandom -> Exp(m_mainTau);
    //delta = 10.5;

    LogInfo<<"evt time delta(s): " << delta<<endl;

    TimeStamp delta_temp(delta);  //use double delta to create a TimeStamp, the construction function, unit s
    TTimeStamp evt_time_delta(delta_temp.GetSec(), delta_temp.GetNanoSec() ); //TimeStamp class provide  more functions,but Sniper use TTimeStamp class, so need convert.

    SniperPtr<IGlobalTimeSvc> TimeSvcPtr(Task::top(),"GlobalTimeSvc");
    IGlobalTimeSvc* TimeSvc = TimeSvcPtr.data();
    TTimeStamp Evt_TimeStamp = TimeSvc->set_current_evt_time(evt_time_delta);// add time delta to the old Evt time

    LogInfo<<"Evt_TimeStamp: " << Evt_TimeStamp<<endl;
    LogInfo<<"Evt_TimeStamp second: " << Evt_TimeStamp.GetSec()<<endl;
    LogInfo<<"Evt_TimeStamp NanoSecond: " << Evt_TimeStamp.GetNanoSec()<<endl;


    package_new_event(Nav, Evt_TimeStamp);

    return true;
}


bool EvtMixingAlg::finalize(){

    return true;
}



string EvtMixingAlg::select()
{
    string sampleName;

    double ranNum = gRandom->Uniform(m_totalRate);
    double sumRate = 0;

    for(map<string,double>::iterator it=m_rateMap.begin(); it!=m_rateMap.end(); it++){
        sumRate += it->second;
        if(ranNum<sumRate){
            sampleName = it->first; 
            break;
        }
    }
    return sampleName;
}


JM::EvtNavigator* EvtMixingAlg::get_one_event(){

    string sample = select();//select sample as input sample name
        LogInfo<<"selected sample: " <<sample<<endl;

    //readin event data
    if( m_firstMap[sample] ){
        int entries = m_incidentMap[sample]->getEntries();
        LogInfo<<"initial selected sample: "<<sample <<", sample entries: " <<entries<<endl;
        int num = gRandom->Integer(entries-1);
        LogInfo<<"first evt num: "<<num<<endl;
        m_incidentMap[sample]->reset(num);//we will read data from this entry
        m_firstMap[sample] = false; //for each sample just need set one time
    }
    m_incidentMap[sample]->fire(); 

    string path = m_taskMap[sample]+":/Event";//m_taskMap[sample] is task name
    LogInfo<<"SniperDataPtr path: " << path<<endl;
    SniperDataPtr<JM::NavBuffer> navBuf(path); 
    if (navBuf.invalid()) {
        LogError << "Can't locate data: " << path << std::endl;
        return 0;
    }
    JM::EvtNavigator* Nav = navBuf->curEvt();
    return Nav;
}


void EvtMixingAlg::package_new_event(JM::EvtNavigator* Nav, TTimeStamp EvtTimeStamp){

    JM::HeaderObject* sh=Nav->getHeader("/Event/SimEvent"); 

    if( sh==0 ){
        LogError<<"failed to get header! "<<endl;
    }
    
    LogDebug<< "have got header! "<< endl;
    JM::SimEvent* se=dynamic_cast<JM::SimEvent*>(sh->event()); 

    TIter next(se->getCDHits());//root's iterator
    JM::SimPMTHit* hit = 0;

    JM::SimEvent* New_se = new JM::SimEvent;//create a new SimEvent to save data

    while( hit=(JM::SimPMTHit*)next() ){
        int pmtID = hit->getPMTID();
        double hitTime = hit->getHitTime();
        JM::SimPMTHit* sph = New_se->addCDHit(); //add Hit in new SimEvent, we create a Hit in vector and return the pointer


        sph->setPMTID(pmtID);
        sph->setHitTime(hitTime);

    }

    JM::SimHeader* New_sh = new JM::SimHeader;
    New_sh->setEvent(New_se);

    JM::EvtNavigator* navigator = new JM::EvtNavigator();

    navigator->setTimeStamp(EvtTimeStamp);  //Sniper require TTimeStamp, but we use TimeStamp. So need to convert. We use TimeStamp because TimeStamp provide some useful function.

    navigator->addHeader("/Event/SimEvent",New_sh);
    m_memMgr_top->adopt( navigator, "/Event"); //put this event object to sniper buffer

}


