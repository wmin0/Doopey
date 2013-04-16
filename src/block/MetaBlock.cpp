#include <string>

#include "block/MetaBlock.h"

using namespace Doopey;
using std::string;

MetaBlock::MetaBlock(unsigned char* data, BlockID id): Block(data, id)
{
  _nameLength = (uint64_t*)data;
  *_nameLength = 0;
  _filename = NULL;
  _ctime = NULL;
  _blockNumber = NULL;
  _firstBlockID = NULL;
}

bool MetaBlock::setFileName(string& name)
{
  //save file name
  *_nameLength = name.length();

  //setup pointer of filename
  if(_filename == NULL)
    _filename = (char*)(_nameLength + sizeof(uint64_t));
  else{
    log->warning("MetaBlock: \
                  change filename means reuse this MetaBlock\n");
    _ctime = NULL;
    _blockNumber = NULL;
    _firstBlockID = NULL;
  }

  //save the filename
  uint64_t l = name.copy(_filename, *_nameLength, 0);

  //check if the copy success
  l -= *_nameLength;
  if(l == 0)
    return true;
  else{
    //copy fail
    log->error("MetaBlock: copy filename error\n");
    *_nameLength = 0;
    _filename = NULL;
    return false;
  }
}

bool MetaBlock::setCreateTime(time_t ctime)
{
  if(_filename == NULL)
  {
    log->error("MetaBlock: error order of setting up MetaBlock, \
                call setFileName first\n");
    return false;
  }

  if(_ctime==NULL)
    _ctime = (time_t*)(_filename + sizeof(char)*(*_nameLength));
  *_ctime = ctime;

  return true;
}

bool MetaBlock::addDataID(BlockID id)
{
  if(_ctime == NULL)
  {
    log->error("MetaBlock: error order of setting up MetaBlock, \
                set up filename and create time first \n");
    return false;
  }
  if(_blockNumber == NULL){
    _blockNumber = (uint64_t*)(_ctime + sizeof(time_t));
    _firstBlockID = (BlockID*)( _blockNumber + sizeof(uint64_t));
    *_blockNumber = 0;
  }
  
  BlockID* save = _firstBlockID + (*_blockNumber)*sizeof(BlockID);
  *_blockNumber = *_blockNumber + 1;
  *save = id;

  return true;
}

string MetaBlock::getFileName() const
{
  if(_filename == NULL)
  {
    log->error("MetaBlock:get filename before set up!\n");
    return "";
  }

  string filename = "";
  filename.append(_filename, *_nameLength);
  return filename;
}

time_t MetaBlock::getCreateTime() const
{
  if(_ctime == NULL)
  {
    log->error("MetaBlock:get create time before set up!\n");
    return 0;
  }
  return *_ctime;
}

uint64_t MetaBlock::getDataBlockNumber() const
{
  if(_blockNumber == NULL)
  {
    log->error("MetaBlock: get number of datas before set up!\n");
    return 0;
  }

  return *_blockNumber;
}

BlockID MetaBlock::getDataBlockID(uint64_t number) const
{
  if(_blockNumber == NULL)
  {
    log->error("MetaBlock: get ID of data block before set up!\n");
    return 0;
  }

  BlockID* id = _firstBlockID + (number-1)*sizeof(BlockID);

  return *id;
}
