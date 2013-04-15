#include "common/Doopey.h"
#include "machine/Server.h"

using namespace Doopey;

int main() {
  SectionCollectionSPtr section = DoopeyInit("");
  Server server(section);
  log->info("Server Start!\n");
  if (server.start()) {
    server.serve();
  }
  DoopeyFinal();
  return 0;
}
