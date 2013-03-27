#include "machine/Server.h"

#include "block/BlockManager.h"
#include "common/Doopey.h"
#include "config/Config.h"
#include "config/ConfigLoader.h"
#include "config/SectionCollection.h"
#include "network/Router.h"

using namespace Doopey;

Server::Server(const char* configPath) {
  log.debug("Hello World!\n");

  ConfigLoader loader;
  _sectionCollection.reset(loader.loadConfig(configPath));
  _blockManager.reset(new BlockManager(_sectionCollection->getConfig("")));
  _router.reset(new Router(_sectionCollection->getConfig("")));
  _router->start();
}

Server::~Server() {
}

bool Server::start() {
  return true;
}
