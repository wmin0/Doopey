#include <functional>
#include <string>

#include "file/FileTree.h"

using namespace Doopey;
using std::string;

template<typename KEY, typename VALUE>
HashList<KEY,VALUE>::HashList(uint64_t size):_listSize(size)
{
  _list = new HashItem*[size];
  for(uint64_t i=0; i<size; i++)
    _list[i] = NULL;
}

template<typename KEY, typename VALUE>
bool HashList<KEY,VALUE>::add(const KEY& key, const VALUE& value)
{
  uint64_t index = keyHash(key)%_listSize;
  if(_list[index] == NULL)//means it is the first element
  {
    _list[index] = new HashItem(key, value, NULL);  
  }else{//means it is not the first
    HashItem* saveplace = _list[index];
    while(saveplace->next != NULL)
    {
      saveplace = saveplace->next;
    }
    saveplace->next = new HashItem(key, value, NULL);
  }
  return true;
}

template<typename KEY, typename VALUE>
bool HashList<KEY,VALUE>::remove(const KEY& key)
{
  uint64_t index = keyHash(key)%_listSize;
  HashItem* pos = _list[index];
  if(_list[index]->key == key)
  {
    _list[index] = _list[index]->next;
    free(pos);
    return true;
  }
  pos = _list[index]->next;
  HashItem* ppos = _list[index];
  while(pos!=NULL)
  {
    if(pos->key == key)
    {
      ppos->next = pos->next;
      free(pos);
      return true;
    }
  }
  return false;
}

template<typename KEY, typename VALUE>
VALUE HashList<KEY, VALUE>::getValue(const KEY& key)
{
  uint64_t index = keyHash(key)%_listSize;
  HashItem* pos = _list[index];
  while(pos!=NULL)
  {
    if(pos->key == key)
      return pos->value;
  }
  return NULL;
}

FileTree::FileTree()
{
}

FileTree::~FileTree()
{
}

