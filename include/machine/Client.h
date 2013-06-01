#ifndef _DOOPEY_CLIENT_H_
#define _DOOPEY_CLIENT_H_

#include <string>

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

      static void receiveBlock(void* wsize, void* nblock, void* filename);
  }; // class Client

};  // namespace Doopey

#endif
