#ifndef __FORMAT_FLV_PARSE_FLV_HEADER_H_
#define __FORMAT_FLV_PARSE_FLV_HEADER_H_

#include "../system_header.h"

struct FlvHeader {
  char f;         // 8bit
  char l;         // 8bit
  char v;         // 8bit
  int version;    // 8bit
  int reserved0;  // 5bit
  int has_audio;  // 1bit
  int reserved1;  // 1bit
  int has_video;  // 1bit
  int dataoffset; // 32bit

  FlvHeader &operator=(const FlvHeader &others) {
    this->f = others.f;
    this->l = others.l;
    this->v = others.v;
    this->version = others.version;
    this->reserved0 = others.reserved0;
    this->has_audio = others.has_audio;
    this->reserved1 = others.reserved1;
    this->has_video = others.has_video;
    this->dataoffset = others.dataoffset;
    return *this;
  }
};





#endif