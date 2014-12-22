#include <iostream>
#include <vector>
#include <exception>
#include <limits>

using namespace std;

class SyncTimeSvc;
SyncTimeSvc* syncsvc;


typedef int PmtId;
// typedef int Time;
typedef long int TimeInterval;
typedef int Adc;


/************************
 *                      *
 *         TIME         *
 *                      *
 ************************/

struct Time {
    static const int s2ns = 1000000000;
    long int s;
    int ns;

    Time() {
        s = 0;
        ns = 0;
    }

    Time(int t2) {
        s = t2/s2ns;
        ns = t2%s2ns;

    }

    // operator int () {
    //     return ns;
    // }

    Time& operator+=(const Time& t2) {
        ns += t2.ns;
        s += t2.s;
        if (ns >= s2ns) {
            s += 1;
            ns -= s2ns;
        }
        return *this;
    }

    Time operator+(const Time& t2) const {
        Time t;
        t.s = s+t2.s;
        t.ns = ns + t2.ns;
        return t;
    }

    Time operator+(const Time& t2) {
        Time t;
        t.s = s+t2.s;
        t.ns = ns + t2.ns;
        return t;
    }

    Time operator+(int t2) {
        Time t;
        //t.s = s+t2.s;
        t.ns = ns + t2;
        while(t.ns > s2ns)
        {
            t.ns -= s2ns;
            t.s += 1;
        }
        return t;
    }
    
    TimeInterval operator-(const Time& t2) {
        // Time t;
        // t.s = s-t2.s;
        // t.ns = ns - t2.ns;

        return (long int)(ns - t2.ns) + (s-t2.s)*s2ns;  
    }

//  TimeInterval operator-(const Time& t2) {
//      Time t;
//      t.s = s-t2.s;
//      t.ns = ns - t2.ns;
//
//      if (t.ns < 0) {
//          t.ns += s2ns;
//          t.s -= 1;
//      }
//
//      if (t.s == 0) {
//          return t.ns;
//      } 
//
//      throw exception();
//  }

    Time& operator++() {
        return operator+=(1);
        
    }

    bool operator<(const Time& t2) const {
        if (s < t2.s) {
            return true;
        } else if (s == t2.s) {
            return (ns < t2.ns);
        } else {
            return false;
        }

    }
};


class SyncTimeSvc {
    private:
        Time m_current_time; 
        Time m_timewindow_start_time; 

        TimeInterval m_circular_buffer_size;
        TimeInterval m_timewindow_size;
    public:

        SyncTimeSvc() {
            m_current_time = 0;
            m_timewindow_start_time = 0;
            m_circular_buffer_size = 1000;
            m_timewindow_size = 1500;
        }

        Time get_current_time() {
            return m_current_time;
        }

        TimeInterval get_circular_buffer_size() {
            return m_circular_buffer_size;
        }

        TimeInterval get_timewindow_size () {
            return m_timewindow_size;
        }

        Time get_timewindow_start_time() {
            return m_timewindow_start_time;
        }

        int get_circular_time () {
            return m_current_time.ns % get_circular_buffer_size();
        }

        void increment_time() {
            ++m_current_time; 
            cout << "SyncTimeSvc >>> incremented time now is " << m_current_time.ns << endl;
        }

        void shift_timewindow() {
            cout << "SyncTimeSvc::shift_timewindow >>> new timewindow_start_time is " << (m_timewindow_start_time + m_timewindow_size).ns << endl;
            m_timewindow_start_time += m_timewindow_size;
        }

        bool is_more_data_needed() {
            // cout << "is_more_data_needed()" << endl;
            // cout << "m_current_time : " << m_current_time.ns << endl;
            // cout << "m_timewindow_start_time : " << m_timewindow_start_time.ns << endl;
            // cout << "return value is : " <<  ((m_current_time - m_timewindow_start_time) == (m_timewindow_size - m_circular_buffer_size)) << endl;
            if((m_current_time - m_timewindow_start_time) == (m_timewindow_size - m_circular_buffer_size))
                cout << "SyncTimeSvc >>> more data needed" << endl;
            return ((m_current_time - m_timewindow_start_time) == (m_timewindow_size - m_circular_buffer_size));
        }

        bool is_time_at_boundary() {
            if((m_current_time - m_timewindow_start_time ) == m_timewindow_size)
                cout << "SyncTimeSvc >>> time is at boundary" << endl;
            return (m_current_time - m_timewindow_start_time ) == m_timewindow_size;
        }


};


/************************
 *                      *
 *         HIT          *
 *                      *
 ************************/

class Hit {
    public:
        Hit() : pmtid(0), time(0) {}
        Hit(PmtId pmtid_, Time time_) : pmtid(pmtid_), time(time_) {};
        PmtId pmtid;
        Time time; 
};

static bool cmp_Hit (const Hit& i,const Hit& j) { return ((i.time)<(j.time)); }
typedef vector<Hit>::iterator Hit_iter;
typedef vector<Hit>::const_iterator Hit_citer;





/************************
 *                      *
 *       CHANNEL        *
 *                      *
 ************************/

class Channel {
    public:
        static const TimeInterval single_channel_buffer_size = 1000;
        vector<Adc> amp;
        int nOngoingPulses;
        vector<int> pulseStartingTime;


        Channel() 
        {
            /// allocate ADC buffer 
            amp.resize(single_channel_buffer_size);

            /// allocate vector storing pulses starting time
            pulseStartingTime.resize(single_channel_buffer_size);

            /// pulse handling
            nOngoingPulses = 0;
        }

        void add_hit()
        {
            //pulseStartingTime += 
            nOngoingPulses++;
        }

        void remove_hit()
        {
            if(nOngoingPulses>0)
                nOngoingPulses--;
        }
};




/************************
 *                      *
 *        EVENT         *
 *                      *
 ************************/

struct Event {
    int event_id;
    Time event_time;
    vector<Hit> m_hits_col;
};




/************************
 *                      *
 *       BUFFERS        *
 *                      *
 ************************/


class HitsBuffer {
    public:
        vector<Hit> m_hits_buffer;

        bool empty()
        {
            return m_hits_buffer.empty();
        }

        Hit & at(int i)
        {
            return m_hits_buffer.at(i);
        }

        vector<Hit> & get_vector()
        {
            return m_hits_buffer;
        }

        bool add_dark_noise()
        {
            return true;
        }

        void unpack_event(const Event& e) 
        {
            for (Hit_citer it = e.m_hits_col.begin(); it != e.m_hits_col.end(); ++it) 
            {
                //const Hit& hit = *it;
                //m_hits_buffer.push_back(Hit( hit.pmtid, hit.time+e.event_time));
                m_hits_buffer.push_back(Hit( it->pmtid, it->time + e.event_time));
            }
            //std::sort(m_hits_buffer.begin(), m_hits_buffer.end(), cmp_Hit);
            //sort_buffer();
            //cout << "unpack_event :: calling sort" << endl;
        }

        // void dispatch() {
        //     while (true) {
        //         break;
        //     }
        // }

        void sort_buffer()
        {
            std::sort(m_hits_buffer.begin(), m_hits_buffer.end(), cmp_Hit);
        }

        void show() {
            std::cout << std::endl;
            std::cout << "Hits Buffer Begin " << std::endl;
            for (vector<Hit>::iterator it = m_hits_buffer.begin();
                    it != m_hits_buffer.end(); ++it) {
                std::cout << it->pmtid << ": " << it->time.ns << std::endl;
            }
            std::cout << "Hits Buffer End " << std::endl << endl;
        }
};




class EventsBuffer {
    public:
        vector<Event> m_events_buffer;

        bool load_events() {
            Event e1;
            e1.event_id = 1;
            e1.event_time = 5;
            e1.m_hits_col.push_back(Hit(0,2));
            e1.m_hits_col.push_back(Hit(1,4));
            e1.m_hits_col.push_back(Hit(0,2000));

            m_events_buffer.push_back(e1);

            Event e2;
            e2.event_id = 2;
            e2.event_time = 10;
            e2.m_hits_col.push_back(Hit(1,1));
            e2.m_hits_col.push_back(Hit(0,100));
            e2.m_hits_col.push_back(Hit(1,3540));

            m_events_buffer.push_back(e2);

            Event e3;
            e3.event_id = 3;
            e3.event_time = 1999;
            e3.m_hits_col.push_back(Hit(1,1));
            e3.m_hits_col.push_back(Hit(0,0));
            e3.m_hits_col.push_back(Hit(1,157));

            m_events_buffer.push_back(e3);

            return !m_events_buffer.empty();
        }

        bool trigger_hits(HitsBuffer& hits_buffer) {
            bool are_there_new_hits = false;
            std::cout << "EventBuffer::trigger_hits >>> using time window starting at: " << syncsvc->get_timewindow_start_time().s << ":" 
                << syncsvc->get_timewindow_start_time().ns << std::endl;
            std::cout << "EventBuffer::trigger_hits >>> and ending at: " << (syncsvc->get_timewindow_start_time() + int(syncsvc->get_timewindow_size()) ).s << ":"
                << (syncsvc->get_timewindow_start_time() + int(syncsvc->get_timewindow_size()) ).ns << endl;
            while (true) 
            {
                if (m_events_buffer.size() == 0) 
                {
                    std::cout << "EventBuffer::trigger_hits >>> Need more hits, but event Buffer is empty." << std::endl;

                    /// stop adding hits because the event buffer is empty
                    break;
                }
                /// read first event in the buffer
                Event& first_event = m_events_buffer[0];
                Time& event_time = first_event.event_time;

                const Time& timewindow_start_time = syncsvc->get_timewindow_start_time();

                cout <<      "EventBuffer::trigger_hits >>> scanning Event Id: " << first_event.event_id << endl;
                std::cout << "EventBuffer::trigger_hits >>> where Event Time is : " << event_time.s << ":" << event_time.ns << std::endl;

                if ((event_time-timewindow_start_time) < syncsvc->get_timewindow_size() ) 
                {
                    // pop the first event, and unpack it
                    std::cout << "EventBuffer::trigger_hits >>> pushing event " << first_event.event_id  << " into the hits buffer" << std::endl;
                    hits_buffer.unpack_event(first_event);
                    m_events_buffer.erase(m_events_buffer.begin());
                    are_there_new_hits = true;
                }
                else 
                {
                    /// stop adding hits because the next event happens to be outside the timewindow
                    std::cout << "EventBuffer::trigger_hits >>> event " << first_event.event_id  
                              << " has not been pushed the hits buffer because it was outside the time window" << std::endl;
                    break;
                }

            }

            if(are_there_new_hits)
            {
                /// sort hits that have just been added
                std::cout << "EventBuffer::trigger_hits >>> sorting hits buffer " << endl;
                hits_buffer.sort_buffer();
                are_there_new_hits = false;
            }
            else
            {
                std::cout << "EventBuffer::trigger_hits >>> hits not sorted beacuse no new hit has been added" << endl;
            }

            hits_buffer.show();

            return !(hits_buffer.empty());
        }
};




class ElecSimAlg {
    private:
        HitsBuffer m_hits_buffer;
        EventsBuffer m_events_buffer;
        vector<Channel> m_all_channels;
        int m_number_of_channels;

    public:
        ElecSimAlg()
        {
            m_number_of_channels = 20;

            /// allocate space for 20 channels
            m_all_channels.resize(m_number_of_channels);
        }

        void run()
        {
            bool status = true;
            int n_processed_events(0);
            int n_loaded_events(0);
            int n_max_events(3);


            status &= m_events_buffer.load_events();
            status &= m_events_buffer.trigger_hits(m_hits_buffer);
            status &= m_hits_buffer.add_dark_noise();

            //m_hits_buffer.show();

            /// time flow
            while(status || syncsvc->is_time_at_boundary())
            {
                cout << endl << "ElecSimAlg::run >>> current time: " << (syncsvc->get_current_time()).ns << "  status : " << status << endl;
                dispacth_hits();
                simulate_one_step();
                syncsvc->increment_time();

                if(syncsvc->is_more_data_needed())
                {
                    cout << "ElecSimAlg::run >>> more data needed >>> shifting time window" << endl;
                    syncsvc->shift_timewindow();
                    status &= m_events_buffer.trigger_hits(m_hits_buffer);
                    cout << "ElecSimAlg::run status of EventsBuffer::trigger_hits is " << status << endl;
                }
                //cout << "after checking if more data is needed" << endl;

            }

                 
        }
        
        void dispacth_hits()
        {
            /// consider multiple hits at the same time
            while(true)
            {
                if(m_hits_buffer.empty() )
                    break;

                else if(m_hits_buffer.at(0).time - syncsvc->get_current_time() != 0)
                    break;

                else //if (m_hits_buffer.at(0).time - syncsvc->get_current_time() == 0 )
                {
                    cout << "ElecSimAlg::dispacth_hits >>> dispatching hit to pmt " << m_hits_buffer.at(0).pmtid << " at time " << m_hits_buffer.at(0).time.ns << endl;
                    m_hits_buffer.get_vector().erase(m_hits_buffer.get_vector().begin());
                }
            }
        }

        void simulate_one_step()
        {
        }
};
 

 //     
 //             void add_dark_noise() {};
 //     
 //             void read_expanded_buffer()
 //             {
 //                 if(m_expanded_buffer.size() < m_min_buffer_size)
 //                 {
 //                     load_compressed_buffer();
 //                     expand_compressed_buffer();
 //                     add_dark_noise();
 //                 }
 //     
 //                 vector < vector<PmtId> >::iterator pmts_at_current_time = m_expanded_buffer.begin();
 //     
 //                 while(pmts_at_current_time != m_expanded_buffer.end())
 //                 {
 //                     if( !(pmts_at_current_time->empty()) )
 //                         dispatch_hits(*pmts_at_current_time);
 //     
 //                     simulate_one_step();
 //                     
 //                     increment_circular_time();
 //                     ++current_linear_time;
 //                     ++pmts_at_current_time;
 //     
 //                     /// debug
 //                     if(current_linear_time==10) break;
 //                 }
 //     
 //             }
 //     
 //         void dispatch_hits( vector<PmtId> & pmts )
 //         {
 //             for(vector<PmtId>::iterator it = pmts.begin() ; it != pmts.end() ; ++it )
 //             {
 //                 m_all_channels.at(*it).add_hit();
 //     
 //                 /// debug
 //                 cout << "current time " << current_linear_time << " - adding hit to pmt " << *it << endl;
 //             }
 //         }
 //         
 //         
 //         void simulate_one_step()
 //         {
 //         }
 //                         
 //         void increment_circular_time()
 //         {
 //         }
 //     
 //     };


/*

class ElecSimAlg {
    public:

        /// each ns there might be hits in several pmts
        vector< vector < PmtId > > m_expanded_buffer; 
        vector<Channel> m_all_channels;

        int m_number_of_channels;
        int m_min_buffer_size;
        int m_max_buffer_size;
        static Time current_circular_time;
        static Time current_linear_time;

        ElecSimAlg()
        {
            m_number_of_channels = 20;

            /// allocate space for 20 channels
            m_all_channels.resize(m_number_of_channels);

            /// time threshold ro refill the event buffer
            m_min_buffer_size = 1;
        }

        void load_compressed_buffer()
        {
            Hit hit1(0, 2);
            Hit hit1_b(0, 4);
            Hit hit2(1, 2);
            Hit hit3(0, 200006);
            Hit hit4(1, 200008);

            m_compressed_buffer.push_back(hit1);
            m_compressed_buffer.push_back(hit1_b);
            m_compressed_buffer.push_back(hit2);
            m_compressed_buffer.push_back(hit3);
            m_compressed_buffer.push_back(hit4);
        }

        int get_max_time()
        {
            int max_time(0);

            for(Hit_iter it = m_compressed_buffer.begin(); it!= m_compressed_buffer.end(); ++it )
            {
                if(it->time > max_time)
                    max_time = it->time;
            }
            return max_time;
        }

        void expand_compressed_buffer()
        {
            int expanded_buffer_size = get_max_time()+1;

            /// this buffer contains the pmts hit at each ns
            if(m_expanded_buffer.size() < expanded_buffer_size)
                m_expanded_buffer.resize(expanded_buffer_size);

            /// reset
            for(vector< vector < PmtId > >::iterator it = m_expanded_buffer.begin(); it != m_expanded_buffer.end(); ++it )
                it->clear();

            /// fill
            for(Hit_iter it = m_compressed_buffer.begin(); it != m_compressed_buffer.end(); ++it )
                m_expanded_buffer.at(it->time).push_back(it->pmtid);

            /// debug dump
            int cnt (0);
            for(vector< vector < PmtId > >::iterator it = m_expanded_buffer.begin(); it != m_expanded_buffer.end(); ++it )
            {
                cout << "[" << cnt << "] :: "; 
                for(vector < PmtId >::iterator itt = it->begin(); itt != it->end(); ++itt)
                    cout << *itt << " ";
                cout << endl;
                ++cnt;

                if (cnt==10) break;
            }

        }

        void add_dark_noise() {};

        void read_expanded_buffer()
        {
            if(m_expanded_buffer.size() < m_min_buffer_size)
            {
                load_compressed_buffer();
                expand_compressed_buffer();
                add_dark_noise();
            }

            vector < vector<PmtId> >::iterator pmts_at_current_time = m_expanded_buffer.begin();

            while(pmts_at_current_time != m_expanded_buffer.end())
            {
                if( !(pmts_at_current_time->empty()) )
                    dispatch_hits(*pmts_at_current_time);

                simulate_one_step();
                
                increment_circular_time();
                ++current_linear_time;
                ++pmts_at_current_time;

                /// debug
                if(current_linear_time==10) break;
            }

        }

    void dispatch_hits( vector<PmtId> & pmts )
    {
        for(vector<PmtId>::iterator it = pmts.begin() ; it != pmts.end() ; ++it )
        {
            m_all_channels.at(*it).add_hit();

            /// debug
            cout << "current time " << current_linear_time << " - adding hit to pmt " << *it << endl;
        }
    }
    
    
    void simulate_one_step()
    {
    }
                    
    void increment_circular_time()
    {
    }

};


Time ElecSimAlg::current_circular_time = 0;
Time ElecSimAlg::current_linear_time = 0;
*/


int main()
{
    syncsvc = new SyncTimeSvc;

    ElecSimAlg myAlg;

    // cout << "int min = " << std::numeric_limits<int>::min() << endl;
    // cout << "int max = " << std::numeric_limits<int>::max() << endl;
    // cout << "long int min = " << std::numeric_limits<long int>::min() << endl;
    // cout << "long int max = " << std::numeric_limits<long int>::max() << endl;

    // return 0 ;

    myAlg.run();

    /*
    ElecSimAlg::current_circular_time = 0;
    ElecSimAlg::current_linear_time = 0;
    ElecSimAlg thisAlg;


    thisAlg.read_expanded_buffer();
    */

    return 0;
}
