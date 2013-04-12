#include "common/Doopey.h"
#include "machine/Server.h"

using namespace Doopey;

int main() {
  DoopeyInit();
  Server server("");
  server.start();
  DoopeyFinal();
  return 0;
}
