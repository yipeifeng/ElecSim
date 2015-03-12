#ifndef WaveformSimAlg_h
#define WaveformSimAlg_h
#include "SniperKernel/AlgBase.h"
#include "EvtNavigator/NavBuffer.h"
#include "Context/TimeStamp.h"
#include "ElecBufferMgrSvc/IElecBufferMgrSvc.h"
#include "GlobalTimeSvc/IGlobalTimeSvc.h"
#include <map>
#include <vector>
#include <string>


namespace JM 
{
    class SimHeader;
    class SimEvent;
}


class WaveformSimAlg: public AlgBase
{
    public:
        WaveformSimAlg(const std::string& name);
        ~WaveformSimAlg();

        bool initialize();
        bool execute();
        bool finalize();


    private:
        void get_Services();
        void clear_vector();
        void load_Pulse();
        void produce_Waveform();


        void mapPulsesByChannel(std::vector<Pulse>& pulse_vector, 
            std::map<int, std::vector<Pulse> >& pulseMap);

        void generateOneChannel(int channelId,
                std::vector<Pulse>& channelPulses);


    private:
        void loadResponse();
        double pmtPulse(double deltaT, int nPulse); 
        double overshoot(double deltaT);




    private:

        JM::SimHeader* m_simheader;
        JM::SimEvent* m_simevent;

        IElecBufferMgrSvc* BufferSvc;
        IGlobalTimeSvc* TimeSvc; 

        std::vector<Pulse> pulse_vector;



        double m_preWaveSimWindow;
        double m_postWaveSimWindow;


        bool m_enableOvershoot;
        bool m_enableSatuation;
        bool m_enableNoise;

        double m_simFrequency;
        double m_noiseAmp;
        double m_speAmp;
        double m_width;
        double m_mu;
        double m_linearityThreshold;
        double m_PulseSampleWith;

        int m_PmtTotal;

        // Ideal PMT pulse shape.
        std::vector<double> m_pmtPulse;
        // Ideal overshoot shape
        std::vector<double> m_overshoot;






};





#endif
