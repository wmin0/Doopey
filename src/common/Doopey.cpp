#include "common/Doopey.h"

#include "common/Thread.h"
#include "logger/Logger.h"
#include "common/ConfigLoader.h"
#include "common/SectionCollection.h"
#include "common/Config.h"
#include "network/Socket.h"

#include <pthread.h>
#include <string>

using namespace Doopey;
using namespace std;

namespace {
  string initRoot(char* argv0) {
    char* path = realpath(argv0, NULL);
    string root(path);
    int count = 0;
    size_t i = root.size() - 1;
    for (; i >= 0; --i) {
      if ('/' == root[i]) {
        if (0 == count) {
          ++count;
        } else {
          break;
        }
      }
    }
    free(path);
    return root.substr(0, i + 1);
  }
};  // namespace


LoggerSPtr Doopey::log(NULL);

const int Doopey::DoopeyPort = 10090;

string Doopey::DoopeyRoot("");

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

SectionCollectionSPtr Doopey::DoopeyInit(const char* path, char* argv0) {
  // TODO: check return type
  DoopeyRoot = initRoot(argv0);
  pthread_mutex_init(&Thread::_lock, NULL);
  pthread_mutex_init(&Thread::_sig_lock, NULL);
  pthread_mutex_init(&Socket::_sig_lock, NULL);
  SectionCollectionSPtr section =
     ConfigLoader::loadConfig((DoopeyRoot +  path).data());
  // TODO: do log init
  log.reset(new Logger(LL_Debug));
  log->info("Root: %s\n", DoopeyRoot.data());
  return section;
}

bool Doopey::DoopeyFinal() {
  pthread_mutex_destroy(&Thread::_lock);
  pthread_mutex_destroy(&Thread::_sig_lock);
  pthread_mutex_destroy(&Socket::_sig_lock);
  return true;
}
