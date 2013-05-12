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
      bool getFileList(const char* name);
  }; // class Client

};  // namespace Doopey

#endif
