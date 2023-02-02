#pragma once

#include "commonHeader.h"

class AvccReader
{
private:
    uint8_t* buffer_{nullptr};
    int buffer_size_{0};
    int cur_pos_{0};
public:
    AvccReader(uint8_t* buffer, int buffer_size) :
        buffer_(buffer), buffer_size_(buffer_size) {}

    Nalu* GetNalu();
};




