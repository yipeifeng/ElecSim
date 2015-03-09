#ifndef PmtParamSvc_h
#define PmtParamSvc_h

#include "SniperKernel/SvcBase.h"
#include "PmtParamSvc/IPmtParamSvc.h"
#include "Context/TimeStamp.h"
#include <TTimeStamp.h>
#include <string>


class PmtData{

    public:
        PmtData(int id,
                double efficiency,
                double gain,
                double sigmaGain,
                double afterPulseProb,
                double prePulseProb,
                double darkRate,
                double timeSpread,
                double timeOffset): 
            m_pmtId (id),
            m_efficiency (efficiency),
            m_gain (gain),
            m_sigmaGain (sigmaGain),
            m_afterPulseProb (afterPulseProb),   
            m_prePulseProb (prePulseProb), 
            m_darkRate (darkRate),
            m_timeSpread (timeSpread),
            m_timeOffset (timeOffset)
    {
    }

        PmtData(const PmtData& others)
            :
                m_pmtId(others.m_pmtId),
                m_efficiency(others.m_efficiency),
                m_gain(others.m_gain),
                m_sigmaGain(others.m_sigmaGain),
                m_afterPulseProb(others.m_afterPulseProb),
                m_prePulseProb(others.m_prePulseProb), 
                m_darkRate(others.m_darkRate),
                m_timeSpread(others.m_timeSpread),
                m_timeOffset(others.m_timeOffset)    
    {
    }
        int pmtId();
        double efficiency();
        double gain();
        double sigmaGain();
        double afterPulseProb();
        double prePulseProb();
        double darkRate();
        double timeSpread();
        double timeOffset();

    private:
        int m_pmtId;
        double m_efficiency;
        double m_gain;
        double m_sigmaGain;
        double m_afterPulseProb;
        double m_prePulseProb; 
        double m_darkRate;
        double m_timeSpread;
        double m_timeOffset;
};



class PmtParamSvc: public IPmtParamSvc, public SvcBase
{
    public:

        PmtParamSvc(const std::string& name);
        ~PmtParamSvc();

        bool initialize();
        bool finalize();
    
        double get_efficiency(int pmtId);
        double get_gain(int pmtId);
        double get_sigmaGain(int pmtId);
        double get_afterPulseProb(int pmtId);
        double get_timeSpread(int pmtId);
        double get_timeOffset(int pmtId);
        double get_darkRate(int pmtId);






    private:


        std::string m_pmtdata_file;
        std::vector<PmtData> pd_vector;
        int m_PmtTotal;

};











#endif
