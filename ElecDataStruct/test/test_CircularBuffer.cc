#include "ElecDataStruct/CircularBuffer.h"
#include <iostream>

int main() {
    typedef ElecData::CircularBuffer<int, 1000> CircularBufferInt;
    CircularBufferInt mybuffer;
    std::cout << CircularBufferInt::get_buffer_size() << std::endl;

    typedef ElecData::CircularBuffer<float, 2000> CircularBufferFloat;
    CircularBufferFloat mybuffer_float;
    std::cout << CircularBufferFloat::get_buffer_size() << std::endl;
}
