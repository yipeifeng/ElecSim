#ifndef ElecPkg_CircularBuffer_h
#define ElecPkg_CircularBuffer_h

#include <cassert>

namespace ElecData {

template<typename T>
struct init_value {
    static const T value;
};

template<> struct init_value<int> { static const int value = 0; };
template<> struct init_value<long int> { static const long int value = 0; };
template<> struct init_value<float> { static const float value = 0.; };
template<> struct init_value<double> { static const double value = 0.; };

template<typename T, int buffer_size=1000>
class CircularBuffer {
    public:
        CircularBuffer() {
            m_circular_time = 0;
            assert(m_buffer_size > 0);
            for (int i = 0; i < m_buffer_size; ++i) {
                m_values[i] = init_value<T>::value;
            }
        }
        static const int get_buffer_size() {
            return m_buffer_size;
        }

        void increment_time(int time_step=1) {
            if (time_step<=0) { time_step = 1; }
            m_circular_time = (m_circular_time+time_step)%m_buffer_size;
        }

        int get_circular_time() {
            return m_circular_time;
        }
    private:
        T m_values[buffer_size];
        int m_circular_time;

        static const int m_buffer_size = buffer_size;

};

}

#endif
