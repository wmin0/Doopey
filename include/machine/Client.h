#ifndef _DOOPEY_CLIENT_H_
#define _DOOPEY_CLIENT_H_

#include <string>
#include "common/Doopey.h"

using std::string;

namespace Doopey {

  class Client {

    public:
      Client();
      ~Client();

      void run(int argc, char** argv);
    private:
      // TODO: change name XD
      void go();
      bool getFileList(const char* name) const;
      bool putFile(const char* filename, const char* dir) const;
      bool addDir(const char* dirName) const;
      bool getFile(const char* filename);

      static void receiveBlock(void* blockInfo, void* filename, void* output);
  }; // class Client

  class BlockInfo {
    public:
      BlockInfo(uint64_t wSize, uint64_t nBlock, BlockID ID, string IP){
        wsize = wSize;
        nblock = nBlock;
        id = ID;
        ip = IP;
      }
      ~BlockInfo();
      uint64_t wsize;
      uint64_t nblock;
      BlockID id;
      string ip;
  }; //class BlockInfo

};  // namespace Doopey

#endif
