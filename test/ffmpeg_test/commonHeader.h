#pragma once

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <vector>

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/timestamp.h"
#include "libavutil/mathematics.h"
}


class Nalu {
private:
    uint8_t* buffer_{nullptr};
    int buffer_size_{0};

    
    
public:
    Nalu(uint8_t* buffer, int buffer_size) :
        buffer_(buffer), buffer_size_(buffer_size) {}

    uint8_t* GetBuffer() {
        return buffer_;
    }

    int GetBufferSize() {
        return buffer_size_;
    }
};


