#include "common/Doopey.h"
#include "machine/Client.h"

using namespace Doopey;

int main(int argc, char** argv) {
  SectionCollectionSPtr section = DoopeyInit("");
  Client client;
  client.run(argc, argv);
  DoopeyFinal();
}
