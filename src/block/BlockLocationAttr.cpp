#include "block/BlockLocationAttr.h"

#include <algorithm>
#include <sstream>
#include <ctime>

using namespace Doopey;
using namespace std;

bool BlockLocationAttr::addMachine(MachineID m) {
  if (machine.size() >= Block::blockReplica) {
    return false;
  }
  for (size_t i = 0; i < machine.size(); ++i) {
    if (m == machine[i]) {
      return false;
    }
  }
  machine.push_back(m);
  sort(machine.begin(), machine.end());
  return true;
}

void BlockLocationAttr::removeMachine(MachineID m) {
  for (size_t i = machine.size(); i >= 0; --i) {
    if (m == machine[i]) {
      machine.erase(i + machine.begin());
    }
  }
}

bool BlockLocationAttr::replaceMachine(MachineID m1, MachineID m2) {
  bool succ = false;
  for (size_t i = 0; i < machine.size(); ++i) {
    if (m1 == machine[i]) {
      machine[i] = m2;
      succ = true;
    }
  }
  sort(machine.begin(), machine.end());
  return succ;
}

string BlockLocationAttr::toString() const {
  stringstream ss("");
  ss << "BlockID: " << block << endl
     << "Status: " << state << endl
     << "time: " << time(0) - t << endl
     << "machines: ";
  for (size_t i = 0; i < machine.size(); ++i) {
    ss << machine[i] << " ";
  }
  ss << endl;
  return ss.str();
}
