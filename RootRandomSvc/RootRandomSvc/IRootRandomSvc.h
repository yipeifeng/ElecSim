#ifndef IRootRandomSvc_h
#define IRootRandomSvc_h


class IRootRandomSvc{
    public:
        virtual ~IRootRandomSvc() = 0;
        virtual void setSeed(int seed) = 0;

};




#endif
