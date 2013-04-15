#ifndef _DOOPEY_H_
#define _DOOPEY_H_

#include "logger/Logger.h"

#include <csignal>
#include <cstdint>
#include <memory>

using std::shared_ptr;

namespace Doopey {

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
  registerClass(Socket);
  registerClass(TaskThread);
  registerClass(Thread);

  extern Logger log;

  // signal
  #define SIGTSTOP SIGRTMAX - 1
  #define SIGRREQ SIGRTMAX - 2

  // general type
  typedef uint64_t BlockID;
  typedef uint32_t LocalBlockID;
  typedef uint32_t MachineID;

  extern MachineID getMachineIDFromBlockID(BlockID block);
  extern MachineID getLocalIDFromBlockID(BlockID block);
  extern BlockID buildBlockID(MachineID machine, LocalBlockID local);

  extern SectionCollectionSPtr DoopeyInit(const char* path);
  extern bool DoopeyFinal();

  extern const int DoopeyPort;
};  // namespace Doopey

#endif
