#include "common/Doopey.h"
#include "machine/Server.h"

using namespace Doopey;

int main(int argc, char** argv) {
  SectionCollectionSPtr section = DoopeyInit("conf/server.conf", argv[0]);
  Server server(section);
  log->info("Server Start!\n");
  if (server.start()) {
    server.serve();
  }
  DoopeyFinal();
  return 0;
}
