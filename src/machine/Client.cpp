#include "machine/Client.h"

#include "common/Doopey.h"
#include "common/Socket.h"
#include "common/Message.h"
#include "common/TaskThread.h"
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
#include <vector>
#include <fcntl.h>

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
    {"rmDir",required_argument, 0, 'm'},
    {"get",  required_argument, 0, 'g'},
    {"help", no_argument,       0, 'h'},
    {0,      0,                 0,  0}
  };
  while (1) {
    int option_index = 0;
    c = getopt_long(argc, argv, "d:r:l:p:g:h", long_options, &option_index);
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
        if(argv[optind]==NULL){
          log->info("Missing destination dir\n");
          return;
        }
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
        cout << "filename=" << optarg << endl;
        getFile(optarg);
        break;
      case 'h':
        cout << "help request" << endl;
        break;
      case 'd':
        cout << "add Dir request" << endl;
        addDir(optarg);
        break;
      case 'r':
        cout << "remove file request" << endl;
        removeFile(optarg);
        break;
      case 'm':
        cout << "remove dir request" << endl;
        break;
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

  string purename = filename;
  if(purename.find_last_of("/") != string::npos)
    purename = purename.substr(purename.find_last_of("/")+1);

  time_t ctime = stat_buf->st_ctime;
  uint64_t size = stat_buf->st_size;
  uint64_t nameLength = purename.length();
  uint64_t dirLength = strlen(dir);
  uint64_t totalLength = nameLength + dirLength;
  if(dir[dirLength-1]!='/')
    totalLength += 1;

  //clean the stat_buf
  free(stat_buf);

  log->info("FileName:%s\nRemote Dir:%s\nctime:%d\n", purename.data(), dir, ctime);

  //first message to let dispatcher transfer socket to fileManager
  MessageSPtr msg(new Message(MT_File, MC_UpFileStart));
  socket.send(msg);

  //second message to set up the Meta block
  msg.reset(new Message(MT_File, MC_UpFileMeta));
  msg->addData((unsigned char*)&totalLength, sizeof(totalLength));
  msg->addData((unsigned char*)dir, dirLength);
  if(dir[dirLength-1]!='/')
    msg->addData((unsigned char*)"/", 1);
  msg->addData((unsigned char*)purename.data(), nameLength);
  msg->addData((unsigned char*)&ctime, sizeof(ctime));
  msg->addData((unsigned char*)&size, sizeof(size));
  socket.send(msg);
  msg = socket.receive();
  if(msg->getCmd() != MC_FileACK){
    if(msg->getCmd() == MC_FileUploadRepeat)
      log->info("Error: file existed\n");
    else
      log->info("Error when uploading meta\n");
    return false;
  }

  //start to send the file content
  log->info("Start transfer data\n");
  char* buf = new char[DataBlock::blockSize];
  size_t cread;
  do{
    cread = fread(buf, 1, DataBlock::blockSize, pFile);
    if(cread == 0){ //the byte of read is 0
      if(feof(pFile)) //have read to end of file
        break;
      else{ //there is a error
        log->info("error while reading file\n");
        delete[] buf;
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
      delete[] buf;
      return false;
    }
  }while(1);

  delete[] buf;

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

bool Client::getFile(const char* filepath){
  Socket socket(ST_TCP);
  if(!socket.connect("localhost", DoopeyPort)){
    log->info("Error when connect to server\n");
    return false;
  }
  log->info("Connect server success\n");
  //split name from filepath
  string filename = filepath;
  filename = filename.substr(filename.find_last_of("/")+1);

  //create local file
  FILE* pFile = fopen(filename.data(), "r");
  if(NULL != pFile){
    log->info("File already in exist\n");
    return false;
  }
  pFile = fopen(filename.data(), "w+");

  log->info("File create success\n");
  //first message to let dispatcher transfer the task to fileManager
  MessageSPtr msg(new Message(MT_File, MC_RequestFile));
  socket.send(msg);

  //ask for meta
  msg.reset(new Message(MT_File, MC_RequestFile));
  msg->addData((unsigned char*)filepath, strlen(filepath));
  socket.send(msg);
  log->info("Send request file %s message\n", filepath);

  msg = socket.receive();
  if(msg->getCmd()!=MC_FileACK){
    log->info("Error when get meta, maybe the file is not exist\n");
    return false;
  }

  uint64_t file_size = 0;
  msg=socket.receive();
  memcpy((unsigned char*)&file_size, msg->getData().data(), sizeof(uint64_t));
  fseek(pFile, file_size, SEEK_SET);
  fclose(pFile);

  uint64_t blockNum = 0;
  msg=socket.receive();
  memcpy((unsigned char*)&blockNum, msg->getData().data(), sizeof(uint64_t));
  log->info("Receive filezize=%d, blockNum=%d\n", file_size, blockNum);

  BlockID id;
  string ip;
  BlockInfo* info;
  vector<BlockInfo*> blocks(blockNum);

  for(unsigned int i=0; i<blockNum; i++){
    msg = socket.receive();

    //get block
    memcpy((unsigned char*)&id, msg->getData().data(), sizeof(BlockID));
    ip.resize(msg->getData().size()-sizeof(BlockID));
    log->info("size of ip = %d\n", msg->getData().size()-sizeof(BlockID));
    memcpy((unsigned char*)&(ip[0]) , msg->getData().data()+sizeof(BlockID),
      msg->getData().size()-sizeof(BlockID));
    log->info("Receive a block info %d, %s\n", id, ip.data());
    info = new BlockInfo(0, i, id, ip);
    blocks[i]=info;
    //append to the local file
  }

  vector<TaskThreadSPtr> threadPool(4);
  size_t threadNum = 4;

  for(size_t i=0; i<threadNum; i++)
    threadPool[i] = TaskThreadSPtr(new TaskThread(Client::receiveBlock));

  uint64_t wsize;

  vector<int> result(blockNum);
  for(unsigned int i=0; i<blockNum;i++)
    result[i] = 0;

  for(unsigned int i=0; i<blockNum;){
    if(file_size > DataBlock::blockSize)
      wsize = DataBlock::blockSize;
    else
      wsize = file_size;

    for(size_t j=0; j<threadNum; j++){
      if(threadPool[j]->isFree()){
        file_size -= wsize;
        blocks[i]->wsize = wsize;
        // TODO: no handle result
        threadPool[i]->runTask(blocks[i], (void*)filename.data(), &(result[i]));
        i++;
        break;
      }
<<<<<<< HEAD
      sleep(1);
      log->debug("lalala\n");
    }
  }
=======
      // TODO: use sleep but no while?
      if(j == threadNum -1 ){
        j=0;
        sleep(1);
      }
      log->debug("lalala\n");
    }
  }
  fclose(pFile);

  int finish = 0;
  while(finish != 1){
    for(unsigned int i=0; i<blockNum; i++){
      finish = result[i];
      if(finish == 0)
        break;
    }
    sleep(1);
  }

>>>>>>> de990b3b26877bf9df554e4459551ee5afafeecb
  log->info("end of get file\n");
  return true;
}

void Client::receiveBlock(void* blockInfo, void* filename, void* output)
{
  int* outPut = (int*)output;
  *outPut = 1;
  BlockInfo* bInfo = (BlockInfo*) blockInfo;
  Socket socket(ST_TCP);
  if (!socket.connect(bInfo->ip, DoopeyPort)) {
    log->error("Error when connect to server\n");
    *outPut = -1;
    return;
  }

  MessageSPtr msg(new Message(MT_Block, MC_RequestBlockData));
  msg->addData((unsigned char*)&bInfo->id, sizeof(BlockID));
  if(socket.send(msg) == false){
    log->info("Send msg to %s fail while requesting block", bInfo->ip.c_str());
    *outPut = -1;
    return;
  }
  msg=socket.receive();
  if(msg == NULL || msg->getCmd()!= MC_BlockACK){
    log->info("Receive block %llu of file %s fail from %s\n", bInfo->id, (char*)filename, bInfo->ip.c_str());
    *outPut = -1;
    return;
  }

  int f = open((char*)filename, O_WRONLY);
  if(f == -1){
    log->warning("Open error of local file %s\n", (char*)filename);
    *outPut = -1;
    return;
  }

  lseek(f, bInfo->nblock * DataBlock::blockSize, SEEK_SET);

  int wrAmount=0, wrOnce;
  while(wrAmount < (int)bInfo->wsize){
    wrOnce = write(f, msg->getData().data() + wrAmount, bInfo->wsize - wrAmount);
    if(wrOnce > 0)
      wrAmount += wrOnce;
    else {
      if (wrOnce == -1)
        *outPut = -1;
      break;
    }
  }
  if(close(f) == -1)
    log->warning("Close error of local file %s\n", (char*)filename);
}

bool Client::removeFile(const char* path) const
{
  Socket socket(ST_TCP);
  if(!socket.connect("localhost", DoopeyPort)){
    log->info("Error when connect to server\n");
    return false;
  }

  MessageSPtr msg(new Message(MT_File, MC_RmFile));
  socket.send(msg);

  msg.reset(new Message(MT_File, MC_RmFile));
  msg->addData((const unsigned char*)path, strlen(path));
  socket.send(msg);

  msg = socket.receive();
  if(msg->getCmd()!=MC_FileACK){
    log->info("Delete file %s fail\n", path);
    return false;
  }
  log->info("Delete file %s success\n", path);
  return true;
}

bool Client::removeDir(const char* path) const
{
  Socket socket(ST_TCP);
  if(!socket.connect("localhost", DoopeyPort)){
    log->info("Error when connect to server\n");
    return false;
  }
  MessageSPtr msg(new Message(MT_File, MC_RmDir));
  socket.send(msg);
  return true;
}
