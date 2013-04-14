#ifndef _DOOPEY_CLIENT_H_
#define _DOOPEY_CLIENT_H_

namespace Doopey {

  class Client {

    public:
      Client();
      ~Client();

      void run(int argc, char** argv);
    private:
      // TODO: change name XD
      void go();
  }; // class Client

};  // namespace Doopey

#endif
