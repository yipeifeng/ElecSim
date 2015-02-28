#ifndef IPmtParamSvc_h
#define IPmtParamSvc_h



class IPmtParamSvc{
    public:
        virtual ~IPmtParamSvc() = 0;
        virtual double get_efficiency(int pmtId) = 0;
        virtual double get_gain(int pmtId) = 0;
        virtual double get_sigmaGain(int pmtId) = 0;
        virtual double get_afterPulseProb(int pmtId) = 0;
        virtual double get_timeSpread(int pmtId) = 0;
        virtual double get_timeOffset(int pmtId) = 0;







};












#endif
