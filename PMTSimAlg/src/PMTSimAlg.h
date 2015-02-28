#ifndef PMTSimAlg_h
#define PMTSimAlg_h
#include "SniperKernel/AlgBase.h"
#include "EvtNavigator/NavBuffer.h"
#include "Context/TimeStamp.h"
#include "ElecBufferMgrSvc/IElecBufferMgrSvc.h"
#include "GlobalTimeSvc/IGlobalTimeSvc.h"
#include "PmtParamSvc/IPmtParamSvc.h"
#include "ElecDataStruct/Hit.h"
#include <map>
#include <vector>
#include <string>


#define NUM_BINS_DIST 96

static double afterPusleTimingDist[NUM_BINS_DIST] = {0.0000000, 0.0012950, 0.0030194, 0.0064297, 0.0142225,
    0.0247712, 0.0417949, 0.0696709, 0.1063259, 0.1485606,
    0.1957973, 0.2465997, 0.2957179, 0.3401377, 0.3798086,
    0.4135611, 0.4437462, 0.4702155, 0.4928434, 0.5123140,
    0.5289294, 0.5432514, 0.5556360, 0.5664919, 0.5758642,
    0.5843533, 0.5920903, 0.5990121, 0.6052796, 0.6111246,
    0.6165560, 0.6215807, 0.6261028, 0.6304032, 0.6346304,
    0.6387545, 0.6425872, 0.6465577, 0.6500606, 0.6534989,
    0.6568080, 0.6602148, 0.6635239, 0.6667457, 0.6700025, 
    0.6737148, 0.6780693, 0.6829002, 0.6879599, 0.6935379, 
    0.6994667, 0.7058650, 0.7121394, 0.7187122, 0.7255085, 
    0.7324600, 0.7394151, 0.7467314, 0.7546551, 0.7630884, 
    0.7718917, 0.7816427, 0.7914601, 0.8023700, 0.8137512, 
    0.8255861, 0.8375781, 0.8498354, 0.8621276, 0.8741685, 
    0.8855410, 0.8962833, 0.9061268, 0.9156126, 0.9243357, 
    0.9320726, 0.9392022, 0.9459897, 0.9519587, 0.9574180, 
    0.9623468, 0.9667851, 0.9708308, 0.9744296, 0.9777980, 
    0.9807231, 0.9835296, 0.9861040, 0.9885404, 0.9906504, 
    0.9926209, 0.9943923, 0.9959998, 0.9974658, 0.9987678, 
    1.0000000};


class PMTSimAlg: public AlgBase
{
    public:
        PMTSimAlg(const std::string& name);
        ~PMTSimAlg();

        bool initialize();
        bool execute();
        bool finalize();

        void load_Hit();
        void produce_Pulse();
        void clear_vector();

        void get_Services();



    private:

        std::vector<Hit> hit_vector;

        IElecBufferMgrSvc* BufferSvc;
        IGlobalTimeSvc* TimeSvc;
        IPmtParamSvc* PmtDataSvc;



        double m_HitBufferLength;   // unit ns
        double m_HitVectorLength;   // unit ns, in each time how many hit convert to pulse.  The time window from firstHitTime in Hitbuffer to (firstHitTime + HitVectorLength).



    private:


        //Property:
        bool m_enableAfterPulse;
        bool m_enableDarkPulse;
        bool m_enableEfficiency;
        bool m_enableAssignGain;
        bool m_enableAssignSigmaGain;
        double m_Gain;
        double m_SigmaGain;



        double m_preTimeTolerance;
        double m_postTimeTolerance;
        double m_expWeight;
        double m_speExpDecay;
        double m_darkRate;
        double m_PmtTotal;

        //internal varible
        // user-defined PDF and associated bin edges for after-pulses
        // PDFs defined wrt main pulse time
        // default is single count in [-50ns,0ns] and [0us,10us], respectively
        std::vector<double> m_afterPulsePdf;
        std::vector<double> m_afterPulseEdges;
        std::vector<double> m_afterPulseTime;


        // Mode for afterpulse amplitude distribution:
        // SinglePE: Fixed single p.e. amplitude with Gaussian smearing
        // PDF: Use amplitude PDF with peak at single p.e and long tail

        std::string m_afterPulseAmpMode;
        std::string m_darkPulseAmpMode;

        // PDFs for afterpulse amplitude
        std::vector<double> m_afterPulseAmpPdf;
        std::vector<double> m_afterPulseAmpEdges;

        // Property: EnableNonlinearAfterpulse
        // Applies nonlinear afterpulsing rate model if true
        bool m_enableNonlinearAfterpulse;
        // Property: LinearAfterpulseThreshold
        // Upper limit of linear afterpulsing in number of PE.
        double m_linearAfterpulseThreshold;

        // Property: EnableVeryLongTimeSuppression
        // Enable suppression of hit times in the far future
        bool m_enableVeryLongTimeSuppression;
        // Property: VeryLongTime
        // Definition of very long time for hit time suppression
        double m_veryLongTime; 


        // electronic simulation beginning and end times
        double m_simTimeEarliest;
        double m_simTimeLatest;
        double m_simDeltaT;

        // DWL: Size of PMT hits counting window
        double m_timeInterval;

        double vertexTime;

        double preTimeTolerance;
        double postTimeTolerance;
        ///////////////////////////////////////////////

        void getAfterPulseAmpPdf(std::vector<double>& pdf);
        void getAfterPulseAmpEdges(std::vector<double>& edges);
        double PulseAmp(double weight,double gain, double sigmaGain);
        Pulse makeAfterPulse(Pulse pulse);
        Pulse makeDarkPulse( int pmtID);
        double NumAfterPulse( const int numPmtHit);
        int PoissonRand(double mean);
        double ConvertPdfRand01 (double rand,std::vector<double> pdf, std::vector<double> edges);







};









#endif
