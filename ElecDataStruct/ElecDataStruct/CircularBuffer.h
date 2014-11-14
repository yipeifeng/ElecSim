#ifndef ElecPkg_CircularBuffer_h
#define ElecPkg_CircularBuffer_h

namespace ElecData {

template<typename T, int buffer_size=1000>
class CircularBuffer {
    public:
        static const int get_buffer_size() {
            return m_buffer_size;
        }
    private:
        T values[buffer_size];

        static const int m_buffer_size = buffer_size;

};

}

#endif
