#include "machine/ServerSnapshot.h"

#include "common/Doopey.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace Doopey;
using namespace std;

const char* ServerSnapshot::_dirpath = ".Doopey";
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
  string temp;
  _dumpFileTree = "";
  while(snapshot >> temp){
    _dumpFileTree = _dumpFileTree + temp;
    _dumpFileTree = _dumpFileTree + "\n";
  }
  snapshot.close();
  return true;
}

bool ServerSnapshot::save() {
  checkDirExist();
  fstream snapshot(DoopeyRoot + _filepath, ios::out);
  if (!snapshot.good()) {
    return false;
  }
  snapshot << _machineID << endl;
  snapshot << _dumpFileTree;
  snapshot.close();
  return true;
}

void ServerSnapshot::checkDirExist() {
  struct stat sb;
  string path = DoopeyRoot + _dirpath;
  if (-1 == stat(path.data(), &sb)) {
    mkdir(path.data(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  }
}
