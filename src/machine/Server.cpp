#include "machine/Server.h"

#include "common/Doopey.h"
#include "logger/Logger.h"
#include "config/Config.h"
#include "config/ConfigLoader.h"
#include "config/SectionCollection.h"

#include <iostream>

using namespace Doopey;

Server::Server(const char* configPath) {
  log.debug("Hello World!\n");

  ConfigLoader loader;
  _sectionCollection = loader.loadConfig(configPath);
}

Server::~Server() {
}

bool Server::start() {
  std::cout <<"134\n";
  return true;
}
