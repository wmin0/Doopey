#ifndef _DOOPEY_H_
#define _DOOPEY_H_

#include "logger/Logger.h"

#include <csignal>
#include <cstdint>
#include <memory>
#include <string>
#include <pthread.h>

using std::shared_ptr;
using std::string;

namespace Doopey {

  #define enum2String(arg) #arg 
  #define registerClass(T) \
    class T; \
    typedef shared_ptr<T> T##SPtr

  registerClass(Block);
  registerClass(BlockLoader);
  registerClass(BlockLocationAttr);
  registerClass(BlockManager);
  registerClass(BlockResolver);
  registerClass(BlockSaver);
  registerClass(BlockUpdater);
  registerClass(FileManager);
  registerClass(FileUploader);
  registerClass(FileTree);
  registerClass(MetaDecoder);
  registerClass(Config);
  registerClass(ConfigLoader);
  registerClass(DataBlock);
  registerClass(DataInfo);
  registerClass(Dispatcher);
  registerClass(Logger);
  registerClass(Message);
  registerClass(MetaBlock);
  registerClass(Router);
  registerClass(SectionCollection);
  registerClass(Server);
  registerClass(ServerSnapshot);
  registerClass(Socket);
  registerClass(TaskThread);
  registerClass(Thread);

  extern LoggerSPtr log;

  extern string DoopeyRoot;

  // signal
  #define SIGTSTOP SIGRTMAX - 1

  // signal lock
  extern pthread_mutex_t DoopeyAlarmLock;

  // general type
  typedef uint64_t BlockID;
  typedef uint32_t LocalBlockID;
  typedef uint32_t MachineID;

  extern MachineID getMachineIDFromBlockID(BlockID block);
  extern MachineID getLocalIDFromBlockID(BlockID block);
  extern BlockID buildBlockID(MachineID machine, LocalBlockID local);

  extern SectionCollectionSPtr DoopeyInit(const char* path, char* argv0);
  extern bool DoopeyFinal();

  extern const int DoopeyPort;
};  // namespace Doopey

#endif
