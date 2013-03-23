#ifndef _DOOPEY_DATA_INFO_H_
#define _DOOPEY_DATA_INFO_H_

namespace Doopey {

  class DataInfo {
    public:
      DataInfo(unsigned char* s, size_t len): data(s), size(len) {}
      unsigned char* data;
      size_t size;
  }; // class DataInfo

};  // namespace Doopey

#endif
