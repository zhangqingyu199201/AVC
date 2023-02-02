#include "avccReader.h"

Nalu* AvccReader::GetNalu() {
    Nalu* element{nullptr};
    if (cur_pos_ < buffer_size_) {
        int size =  (buffer_[cur_pos_] << 24) | (buffer_[cur_pos_ + 1] << 16)
            | (buffer_[cur_pos_ + 2] << 8) | (buffer_[cur_pos_ + 3]);

        element = new Nalu(buffer_ + cur_pos_, size + 4);   
        cur_pos_ += 4 + size;
    }

    return element;
}
