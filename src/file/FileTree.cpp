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
HashList<KEY, VALUE>::~HashList()
{
  for(uint64_t i=0; i<_listSize; i++)
  {
    HashItem* release = _list[i];
    HashItem* next;
    while(release!=NULL)
    {
      next = release->next;
      free(release);
      release = next;
    }
  }
  free(_list);
}

template<typename KEY, typename VALUE>
bool HashList<KEY,VALUE>::add(const KEY& key, const VALUE& value)
{
  uint64_t index = keyHash(key)%_listSize;
  if(_list[index] == NULL)//means it is the first element
  {
    _list[index] = new HashItem();
    _list[index]->key = key;
    _list[index]->value = value;
    _list[index]->next = NULL;  
  }else{//means it is not the first
    HashItem* saveplace = _list[index];
    while(saveplace->next != NULL)
    {
      saveplace = saveplace->next;
    }
    saveplace->next = new HashItem();
    saveplace = saveplace->next;
    saveplace->key = key;
    saveplace->value = value;
    saveplace->next = NULL;
    
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

TreeNode::TreeNode()
{
  _isFile = 0;
  _name = "";
  _children = NULL;
  _next = NULL;
  _last = NULL;
  _metaID = 0;
}

BlockID TreeNode::getID() const
{
  return _metaID;
}

FileTree::FileTree()
{
  _root = new TreeNode();
  _root->_name = "/";
  _fileMap = new HashList<string, TreeNode*>();
}

FileTree::~FileTree()
{
}

string FileTree::getFirst(string& s)
{
  size_t idx = s.find_first_of("/");
  string get = s.substr(0, idx);
  s.erase(0, idx+1);
  return get;
}

string FileTree::getFileName(const string& path) const
{
  return path.substr(path.find_last_of("/")+1);
}

TreeNode* FileTree::searchChild(TreeNode* node, const string& name) const
{
  if(node == NULL)
    return NULL;
  TreeNode* idx = node->_children;
  while(idx!=NULL)
  {
    if(idx->_name == name)
      return idx;
    else
      idx = idx->_next;
  }
  return idx;
}

TreeNode* FileTree::searchDir(const string& dirPath)
{
  if(dirPath=="")
    return _root;
  string path = dirPath;
  string name = getFileName(dirPath);
  TreeNode* idx = _root;
  while(1)
  {
    string searchStr = getFirst(path);
    if(name == searchStr)
      return searchChild(idx, searchStr);
    idx = searchChild(idx, searchStr);
    if(idx==NULL)
      return NULL;
  }
}

bool FileTree::addFile(const string& path, const BlockID metaID)
{
  string dir = path.substr(0, path.find_last_of("/"));
  string name = getFileName(path);
  TreeNode* insert = searchDir(dir);

  if(insert == NULL)
    return false;

  if(insert->_isFile == true)
    return false;

  insert->_next = new TreeNode();
  (insert->_next)->_last = insert;
  insert = insert->_next;
  insert->_name = name;
  insert->_isFile = metaID!=0?true:false;
  insert->_metaID = metaID;

  _fileMap->add(path, insert);
  return true;
}

bool FileTree::addDir(const string& path)
{
  return addFile(path, 0);
}

bool FileTree::removeNode(TreeNode* tn)
{
  if(tn==NULL)
    return false;

  if(tn->_last != NULL)
    (tn->_last)->_next = tn->_next;
  if(tn->_next != NULL)
    (tn->_next)->_last = tn->_last;

  free(tn);
  return true;
}

bool FileTree::removeFile(const string& path)
{
  TreeNode* remove = _fileMap->getValue(path);
  if(remove == NULL)
    return false;

  if(remove->_isFile == false)
    return false;

  bool result = _fileMap->remove(path);
  if(result == false)
    return false;

  removeNode(remove);

  return true;
}

bool FileTree::removeDir(const string& path)
{
  TreeNode* remove = _fileMap->getValue(path);
  if(remove == NULL)
    return false;

  if(remove->_isFile == true)
    return false;

  bool result = _fileMap->remove(path);
  if(result == false)
    return false;

  if(remove->_children != NULL)
  {
    TreeNode* next = remove->_children;
    while(next!=NULL)
    {
      next->_isFile? removeFile(next->_name):removeDir(next->_name);
      next = remove->_children;
    }
  }
  removeNode(remove);

  return true;
}

BlockID FileTree::getMetaID(const string& filePath)
{ 
  TreeNode* tn = _fileMap->getValue(filePath);
  if(tn == NULL)
    return 0;
  else
    return tn->getID();
}
