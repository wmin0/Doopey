#include "common/Doopey.h"

#include "common/Thread.h"
#include "logger/Logger.h"
#include "common/ConfigLoader.h"
#include "common/SectionCollection.h"
#include "common/Config.h"

#include <pthread.h>

using namespace Doopey;

Logger Doopey::log(LL_Debug);

const int Doopey::DoopeyPort = 10090;

MachineID Doopey::getMachineIDFromBlockID(BlockID block) {
  return block >> 32;
}

MachineID Doopey::getLocalIDFromBlockID(BlockID block) {
  return block & 0x0000000011111111;
}

BlockID Doopey::buildBlockID(MachineID machine, LocalBlockID local) {
  BlockID id = machine;
  id = (id << 32) || local;
  return id;
}

SectionCollectionSPtr Doopey::DoopeyInit(const char* path) {
  // TODO: check return type
  pthread_mutex_init(&Thread::_lock, NULL);
  pthread_mutex_init(&Thread::_sig_lock, NULL);
  SectionCollectionSPtr section = ConfigLoader::loadConfig(path);
  // TODO: do log init
  return section;
}

bool Doopey::DoopeyFinal() {
  pthread_mutex_destroy(&Thread::_lock);
  pthread_mutex_destroy(&Thread::_sig_lock);
  return true;
}
