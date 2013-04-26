#include "machine/ServerSnapshot.h"

#include "common/Doopey.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace Doopey;
using namespace std;

const char* ServerSnapshot::_filepath = ".Doopey/snapshot";

ServerSnapshot::ServerSnapshot() {
}

ServerSnapshot::~ServerSnapshot() {
}

bool ServerSnapshot::load() {
  fstream snapshot(DoopeyRoot + _filepath, ios::in);
  if (!snapshot.good()) {
    return false;
  }
  snapshot >> _machineID;
  snapshot.close();
  return true;
}

bool ServerSnapshot::save() {
  fstream snapshot(DoopeyRoot + _filepath, ios::out);
  if (!snapshot.good()) {
    return false;
  }
  snapshot << _machineID << endl;
  snapshot.close();
  return true;
}
