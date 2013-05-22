#include "machine/Client.h"

#include "common/Doopey.h"
#include "common/Socket.h"
#include "common/Message.h"
#include "block/DataBlock.h"

#include <iostream>
#include <memory>
#include <memory.h>
#include <string>
#include <cstdint>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <getopt.h>

using namespace Doopey;
using namespace std;

typedef shared_ptr<Message> MessageSPtr;

Client::Client() {
}

Client::~Client() {
}

void Client::run(int argc, char** argv) {
  int c = 0;
  struct option long_options[] = {
    {"ls",   required_argument, 0, 'l'},
    {"put",  required_argument, 0, 'p'},//use two parameter
    {"adDir",required_argument, 0, 'd'},
    {"rm",   required_argument, 0, 'r'},
    {"get",  required_argument, 0, 'g'},
    {"help", no_argument,       0, 'h'},
    {0,      0,                 0,  0}
  };
  while (1) {
    int option_index = 0;
    c = getopt_long(argc, argv, "l:p:g:h", long_options, &option_index);
    if (-1 == c) {
      break;
    }
    switch (c) {
      // TODO: build option and check one option only
      case 'l':
        cout << "ls request" << endl;
        cout << "arg=" << optarg << endl;
        getFileList(optarg);
        break;
      case 'p':
        cout << "put request" << endl;
        cout << "filename=" << optarg << endl;
        cout << "optind=" << optind << endl;
        if(argv[optind][0] == '-'){
          cout << "missing the location on remote" << endl;
          return;
        }
        cout << "remote dir=" << argv[optind] << endl;
        putFile(optarg, argv[optind]);
        optind++;
        break;
      case 'g':
        cout << "get request" << endl;
        break;
      case 'h':
        cout << "help request" << endl;
        break;
      case 'd':
        cout << "add Dir request" << endl;
        addDir(optarg);
        break;
      case 'r':
        cout << "remove file/dir request" << endl;
      case '?':
        break;
      default:
        cout << "?? getopt returned character code 0" << c << "??" << endl;
    }
  }
  if (optind < argc) {
    cout << "other arg: ";
    while (optind < argc) {
      cout << argv[optind++] << " ";
    }
    cout << endl;
  }

  //go();
}

void Client::go() {
  Socket socket(ST_TCP);
  if (!socket.connect("localhost", DoopeyPort)) {
    cerr << "connect server error" << endl;
    return;
  }
  MessageSPtr msg(new Message(MT_Machine, MC_RequestSysInfoMem));
  socket.send(msg);
  MessageSPtr ack = socket.receive();
  //string test;
  //uint64_t len;
  //memcpy(&len, ack->getData().data(), sizeof(uint64_t));
  //cout << "len: " << len << endl;
  //test.resize(len);
  //memcpy(&(test[0]), ack->getData().data() + sizeof(uint64_t), len);
  //cout << "test: " << test << endl;
  int mem;
  memcpy(&mem, ack->getData().data(), sizeof(int));
  cout << "mem: " << mem << endl;
}

bool Client::getFileList(const char* name) const
{
  Socket socket(ST_TCP);
  if(!socket.connect("localhost", DoopeyPort)){
    cerr << "connect server error" << endl;
    return false;
  }
  MessageSPtr msg(new Message(MT_File, MC_RequestList));
  socket.send(msg);

  uint64_t l = strlen(name);
  msg.reset(new Message(MT_File, MC_RequestList));
  msg->addData((unsigned char*)name, 0, l);
  socket.send(msg);

  MessageSPtr reply = socket.receive();

  for(unsigned int i=0; i<reply->getData().size(); i++)
    cout << reply->getData()[i];
  socket.close();
  return true;
}

bool Client::putFile(const char* filename, const char* dir) const
{
  Socket socket(ST_TCP);
  if(!socket.connect("localhost", DoopeyPort)){
    cerr << "connet server error" << endl;
    return false;
  }

  //read file
  FILE* pFile;
  pFile = fopen(filename, "rb");
  if(pFile == NULL){
    cerr << "File error\n" << endl;
    socket.close();
    return false;
  }

  //get ctime
  struct stat* stat_buf = (struct stat*)malloc(sizeof(struct stat));
  if(0 != lstat(filename, stat_buf)){
    cerr << "open " << filename << " error" << endl;
    free(stat_buf);
    return false;
  }

  //check the upload filename is really a file
  if( S_ISDIR( stat_buf->st_mode)){
    cerr << filename << " is a dir" << endl;
    free(stat_buf);
    return false;
  }

  time_t ctime = stat_buf->st_ctime;
  uint64_t nameLength = strlen(filename);
  uint64_t dirLength = strlen(dir);
  uint64_t totalLength = nameLength + dirLength;
  if(dir[dirLength-1]!='/')
    totalLength += 1;

  //clean the stat_buf
  free(stat_buf);

  log->info("FileName:%s\nRemote Dir:%s\nctime:%d\n", filename, dir, ctime);

  //first message to let dispatcher transfer socket to fileManager
  MessageSPtr msg(new Message(MT_File, MC_UpFileStart));
  socket.send(msg);

  //second message to set up the Meta block
  msg.reset(new Message(MT_File, MC_UpFileMeta));
  msg->addData((unsigned char*)&totalLength, sizeof(nameLength));
  msg->addData((unsigned char*)dir, dirLength);
  if(dir[dirLength-1]!='/')
    msg->addData((unsigned char*)"/", 1);
  msg->addData((unsigned char*)filename, nameLength);
  msg->addData((unsigned char*)&ctime, sizeof(ctime));
  socket.send(msg);
  msg = socket.receive();
  if(msg->getCmd() != MC_FileACK){
    log->info("Error when uploading meta\n");
    return false;
  }

  //start to send the file content
  log->info("Start transfer data\n");
  char* buf = (char*)malloc(DataBlock::blockSize);
  size_t cread;
  do{
    cread = fread(buf, 1, DataBlock::blockSize, pFile);
    if(cread == 0){ //the byte of read is 0
      if(feof(pFile)) //have read to end of file
        break;
      else{ //there is a error
        log->info("error while reading file\n");
        return false;
      }
    }

    log->info("Read %d bytes and trasmit it\n", cread);

    //prepare the msg of data
    msg.reset(new Message(MT_File, MC_UpFileData));
    msg->addData((unsigned char*)buf, cread);
    socket.send(msg);
    msg = socket.receive();
    if(msg->getCmd() != MC_FileACK){
      log->info("Error when uploading data\n");
      return false;
    }
  }while(1);

  //last msg to end the transfer
  log->info("Finish send the file\n");
  msg.reset(new Message(MT_File, MC_UpFileEnd));
  socket.send(msg);
  msg = socket.receive();
  if(msg->getCmd() != MC_FileACK){
    log->info("Error when adding file into fileTree\n");
    return false;
  }

  return true;
}

bool Client::addDir(const char* dirName) const
{
  Socket socket(ST_TCP);
  if(!socket.connect("localhost", DoopeyPort)){
    log->info("Error when connect to server\n");
    return false;
  }

  MessageSPtr msg(new Message(MT_File, MC_NewDir));
  socket.send(msg);

  msg.reset(new Message(MT_File, MC_NewDir));
  msg->addData((unsigned char*)dirName, strlen(dirName));
  socket.send(msg);

  msg=socket.receive();
  if(msg->getCmd() != MC_FileACK){
    log->info("Error when add new directory\n");
    return false;
  }

  return true;
}
