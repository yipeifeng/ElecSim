#include "ElecDataStruct/CircularBuffer.h"
#include <iostream>

int main() {
    typedef ElecData::CircularBuffer<int, 1000> CircularBufferInt;
    CircularBufferInt mybuffer;
    std::cout << CircularBufferInt::get_buffer_size() << std::endl;
}
