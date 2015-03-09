#include "ElecBufferMgrSvc.h"
#include "SniperKernel/SvcFactory.h"
#include "SniperKernel/SniperLog.h"
#include "Context/TimeStamp.h"
#include "ElecDataStruct/Hit.h"
#include <iterator>

using namespace std;

DECLARE_SERVICE(ElecBufferMgrSvc);


bool SortByHitTime(const Hit& hit1,const Hit& hit2){ 
    return hit1.hitTime()<hit2.hitTime(); 
}

bool SortByPulseTime(const Pulse& pulse1,const Pulse& pulse2){
    return pulse1.pulseHitTime()<pulse2.pulseHitTime();
}



ElecBufferMgrSvc::ElecBufferMgrSvc(const std::string& name) : SvcBase(name) 
{

}


ElecBufferMgrSvc::~ElecBufferMgrSvc(){

}


bool ElecBufferMgrSvc::initialize(){
    HitBuffer.clear();
    LogInfo<<"clear the HitBuffer!"<<endl;

    PulseBuffer.clear();
    LogInfo<<"clear the PulseBuffer!"<<endl;

    TriggerBuffer.clear();
    LogInfo<<"clear the TriggerBuffer!"<<endl;

    return true;
}



bool ElecBufferMgrSvc::finalize(){


    return true;
}

//Hit Buffer

void ElecBufferMgrSvc::save_to_HitBuffer(Hit hit){
    HitBuffer.push_back(hit);

}


deque<Hit>& ElecBufferMgrSvc::get_HitBuffer(){

    return HitBuffer;
}


void ElecBufferMgrSvc::SortHitBuffer(){

    sort(HitBuffer.begin(),HitBuffer.end(), SortByHitTime);


}


TimeStamp ElecBufferMgrSvc::get_firstHitTime(){
    TimeStamp firstHitTime = HitBuffer[0].hitTime(); 

    return firstHitTime;
}


TimeStamp ElecBufferMgrSvc::get_lastHitTime(){
    TimeStamp lastHitTime = HitBuffer.back().hitTime(); 

    return lastHitTime;
}

int ElecBufferMgrSvc::get_HitBufferSize(){

    return HitBuffer.size();
}



std::vector<Hit> ElecBufferMgrSvc::get_HitVector(double TimeLength){

    vector<Hit> tem_HitVector;
    TimeStamp tem_firstHitTime = HitBuffer.front().hitTime();

    TimeStamp delta_temHitTime(0);

    while(delta_temHitTime.GetSeconds()*1e9 < TimeLength && HitBuffer.size() != 0){

        //LogInfo<<"Hit Time: " << HitBuffer.front().hitTime()<<endl;

        tem_HitVector.push_back(HitBuffer.front());
        HitBuffer.pop_front();

        delta_temHitTime = HitBuffer.front().hitTime() - tem_firstHitTime;
        //LogInfo<<"delta_temHitTime(ns): " << delta_temHitTime.GetSeconds()*1e9<<endl;

    }
    LogInfo<<"delta_temHitTime: " << delta_temHitTime.GetSeconds()*1e9<<endl;

    LogInfo<<"HitBuffer size after get_HitVector:" <<HitBuffer.size()<<endl;


    return tem_HitVector;


}








//Pulse Buffer

deque<Pulse>& ElecBufferMgrSvc::get_PulseBuffer(){

    return PulseBuffer;
}


TimeStamp ElecBufferMgrSvc::get_firstPulseTime(){
    TimeStamp firstPulseTime = PulseBuffer[0].pulseHitTime(); 

    return firstPulseTime;
}



TimeStamp ElecBufferMgrSvc::get_lastPulseTime(){
    TimeStamp lastPulseTime = PulseBuffer.back().pulseHitTime(); 
    return lastPulseTime;
}


void ElecBufferMgrSvc::save_to_PulseBuffer(Pulse pulse){
    PulseBuffer.push_back(pulse);
}


int ElecBufferMgrSvc::get_PulseBufferSize(){

    return PulseBuffer.size();
}


void ElecBufferMgrSvc::SortPulseBuffer(){

    sort(PulseBuffer.begin(),PulseBuffer.end(), SortByPulseTime);

 //   deque<Pulse>::iterator it;
 //   for(it = PulseBuffer.begin();
 //           it != PulseBuffer.end();
 //           it++){
 //       LogInfo<<"PulseHitTime: "  << it->pulseHitTime().GetSeconds() * 1e9<<endl;
 //   
 //   
 //   }


}





//Trigger Buffer

deque<TimeStamp>& ElecBufferMgrSvc::get_TriggerBuffer(){
    return TriggerBuffer;
}

void ElecBufferMgrSvc::save_to_TriggerBuffer(TimeStamp TriggerTime){
    TriggerBuffer.push_back(TriggerTime);

}



TimeStamp ElecBufferMgrSvc::get_TriggerTimeStamp(){
    TimeStamp TriggerTimeStamp =  TriggerBuffer.front();
    //TriggerBuffer.pop_front();
    return TriggerTimeStamp;
}



void ElecBufferMgrSvc::pop_TriggerTimeStamp(){
    TriggerBuffer.pop_front();
}




