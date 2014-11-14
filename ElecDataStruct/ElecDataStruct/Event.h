#ifndef ElecPkg_Event_h
#define ElecPkg_Event_h

namespace ElecData {

class Event {
    public:
        Event();

        int getEventId();

    private:
        int m_event_id;
};

}

#endif
