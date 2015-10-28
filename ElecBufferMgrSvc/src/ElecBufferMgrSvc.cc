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


ChannelData::ChannelData(){

    m_ChannelBuffer.resize(30000, 0 );//initial Buffer 

}



ChannelData::ChannelData(int BufferSize){

    m_ChannelBuffer.resize(BufferSize, 1 );//initial Buffer 

}

ChannelData::~ChannelData(){

}


ElecBufferMgrSvc::ElecBufferMgrSvc(const std::string& name) : SvcBase(name) 
{
    TimeStamp temp_stamp(0);
    declProp("PmtTotal",m_PmtTotal=17746);
    declProp("WaveformBufferSize",m_WaveformBufferSize=30000);  
    declProp("start",standard_TimeStamp = temp_stamp);

    standard_Index = 0;

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
    WaveformBuffer.clear();
    LogInfo<<"clear the WaveformBuffer!"<<endl;

    init_WavefromBuffer(m_PmtTotal);
    LogInfo<<"initialize the WaveformBuffer!"<<endl;







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
    LogInfo<<"delta_temHitTime(ns): " << delta_temHitTime.GetSeconds()*1e9<<endl;

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



vector<Pulse> ElecBufferMgrSvc::get_PulseVector(TimeStamp WaveSimLastTime){

    vector<Pulse> tem_PulseVector;
    TimeStamp tem_firstPulseTime = PulseBuffer.front().pulseHitTime();


    LogInfo<<"tem_firstPulseTime(ns): "<<tem_firstPulseTime.GetSeconds()*1e9<<endl;

    //before this step,I have cleared the pulse before WaveSimfirstTime

    while(tem_firstPulseTime < WaveSimLastTime){
        tem_PulseVector.push_back(PulseBuffer.front() ); 
        PulseBuffer.pop_front();

        tem_firstPulseTime = PulseBuffer.front().pulseHitTime();

        //LogInfo<<"tem_firstPulseTime(ns): "<<tem_firstPulseTime.GetSeconds()*1e9<<endl;

    }
    LogInfo<<"tem_PulseVector size: " <<tem_PulseVector.size()<<endl;

    return tem_PulseVector;
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


void ElecBufferMgrSvc::pop_PulseBufferFront(){
    PulseBuffer.pop_front();
}





//Waveform Buffer

void ElecBufferMgrSvc::init_WavefromBuffer(int PmtTotal){

    LogInfo<<"WaveBuffer PMT num before init: "<<WaveformBuffer.size()<<endl;

    ChannelData m_ChannelData(m_WaveformBufferSize); //Set Buffer size

    for(int i=0; i<PmtTotal; i++){
        WaveformBuffer[i] = m_ChannelData; 
    }

    LogInfo<<"WaveBuffer PMT num after init: "<<WaveformBuffer.size()<<endl;

    /*
       LogInfo<<"WaveformBuffer[100] ChannelBufferSize: " << WaveformBuffer[100].ChannelBuffer().size()<<endl;


       TimeStamp standard_TimeStamp(47*1e-9);
       LogInfo<<"standard_TimeStamp: " << standard_TimeStamp.GetSeconds()*1e9<<endl;

       int standard_Index = 4;

       for(int i=40; i<50; i++){
       TimeStamp ValueTime(i*1e-9);
       double ValueAmp = i;
       WaveformBuffer[100].save_value(m_WaveformBufferSize, standard_TimeStamp, standard_Index, ValueTime, ValueAmp); 

       }
       for(int i=0; i<10; i++){
       LogInfo<<WaveformBuffer[100].ChannelBuffer()[i] << endl;

       }
       */


}


void ElecBufferMgrSvc::save_waveform(int channelId, TimeStamp index_stamp, double amplitude){

       WaveformBuffer[channelId].save_value(m_WaveformBufferSize, standard_TimeStamp, standard_Index, index_stamp, amplitude); 

}



void ElecBufferMgrSvc::set_standard_TimeStamp(TimeStamp time){

    if(time > standard_TimeStamp){

        standard_Index += int( (time - standard_TimeStamp).GetSeconds()*1e9 );

        standard_TimeStamp = time;
    }

};







// ChannelData
vector<double>& ChannelData::ChannelBuffer(){
    return m_ChannelBuffer;
}



void ChannelData::save_value(int BufferSize, TimeStamp standard_TimeStamp, int  standard_Index, TimeStamp ValueTime, double ValueAmp){

    double i_standardTime = standard_TimeStamp.GetSeconds()*1e9; 
    double i_ValueTime = ValueTime.GetSeconds()*1e9; //unit ns

    int buffer_index = int(i_ValueTime - i_standardTime + standard_Index) % BufferSize;

    if(buffer_index < 0){
        buffer_index+=BufferSize; 
    }


    //LogInfo<<"ValueTime: " <<i_ValueTime<<endl;
    //LogInfo<<"BufferSize: " << BufferSize<<endl;
    //LogInfo<<"buffer_index: " <<buffer_index<<endl;

    if(ValueTime <= standard_TimeStamp){
        m_ChannelBuffer[buffer_index]+=ValueAmp;
    }else{
        m_ChannelBuffer[buffer_index]=ValueAmp;

    }

}






