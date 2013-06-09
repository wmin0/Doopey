#include "common/Doopey.h"

#include "common/Thread.h"
#include "logger/Logger.h"
#include "common/ConfigLoader.h"
#include "common/SectionCollection.h"
#include "common/Config.h"
#include "common/Socket.h"
#include "block/BlockResolver.h"

#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <string>
#include <csignal>
#include <map>
#include <vector>

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

  typedef pair<sighandler_t, time_t> AlarmMapPair;
  typedef map<sighandler_t, time_t> AlarmMap;
  AlarmMap alarmMap;
  void alarmHandler(int sig) {
    // check
    AlarmMap::iterator it;
    vector<sighandler_t> todo;
    time_t min = -1;
    sighandler_t next = NULL;

    // pickup next
    for (it = alarmMap.begin(); it != alarmMap.end(); ++it) {
      if (time(NULL) >= it->second) {
        it->first(sig);
        todo.push_back(it->first);
      } else {
        if (it->second < min) {
          min = it->second;
          next = it->first;
        }
      }
    }
    for (size_t i = 0; i < todo.size(); ++i) {
      alarmMap.erase(todo[i]);
    }
    if (next) {
      alarm(min - time(NULL));
    }
  }

};  // namespace


LoggerSPtr Doopey::log(NULL);

const int Doopey::DoopeyPort = 10090;

string Doopey::DoopeyRoot("");

void Doopey::DoopeyAlarm(sighandler_t func, time_t sec) {
  if (0 == sec) {
    // erase
    AlarmMap::iterator it = alarmMap.find(func);
    if (alarmMap.end() == it) {
      return;
    }
    alarmMap.erase(it);
  } else {
    alarmMap[func] = time(NULL) + sec;
  }
  alarm(0);
  alarmHandler(SIGALRM);
}

MachineID Doopey::getMachineIDFromBlockID(BlockID block) {
  return block >> 32;
}

LocalBlockID Doopey::getLocalIDFromBlockID(BlockID block) {
  return block;
}

BlockID Doopey::buildBlockID(MachineID machine, LocalBlockID local) {
  BlockID id = machine;
  id = (id << 32) + local;
  //log->debug("buildBlockID: %d %d -> %lld\n", machine, local);
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
  srand(time(NULL));
  signal(SIGALRM, alarmHandler);
  return section;
}

bool Doopey::DoopeyFinal() {
  pthread_mutex_destroy(&Thread::_lock);
  pthread_mutex_destroy(&Thread::_sig_lock);
  pthread_mutex_destroy(&Socket::_sig_lock);

  signal(SIGALRM, NULL);
  return true;
}
