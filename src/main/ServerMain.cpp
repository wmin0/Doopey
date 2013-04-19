#include "common/Doopey.h"
#include "machine/Server.h"

using namespace Doopey;

int main(int argc, char** argv) {
  SectionCollectionSPtr section = DoopeyInit("conf/server.conf", argv[0]);
  Server server(section);
  if (server.start()) {
    log->info("Server Start!\n");
    server.serve();
  }
  DoopeyFinal();
  return 0;
}
