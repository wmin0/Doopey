#include "common/Doopey.h"
#include "machine/Server.h"

using namespace Doopey;

int main() {
  DoopeyInit();
  Server server("");
  log.info("Server Start!\n");
  server.start();
  DoopeyFinal();
  return 0;
}
