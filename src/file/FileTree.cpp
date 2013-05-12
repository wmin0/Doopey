#include <functional>
#include <string>
#include <vector>

#include "file/FileTree.h"

using namespace Doopey;
using std::string;
using std::vector;

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
  while (idx == 0){
    s.erase(0, 1);
    idx = s.find_first_of("/");
  }
    
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

  if(insert->_children != NULL)
  {
    insert = insert->_children;
    while(insert->_next != NULL)
      insert = insert->_next;
    insert->_next = new TreeNode();
    (insert->_next)->_last = insert;
    insert = insert->_next;
  }else{
    insert->_children = new TreeNode();
    (insert->_children)->_parent = insert;
    insert = insert->_children;
  }
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
  if( (tn->_parent)->_children == tn)
    (tn->_parent)->_children = tn->_next;

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
      next = remove->_next;
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
vector<string> FileTree::getChildren(const string& filePath)
{
  TreeNode* tn = _fileMap->getValue(filePath);
  vector<string> result;
  if(tn != NULL){
    TreeNode* working = tn->_children;
    while(working!=NULL){
      result.insert(result.begin(), working->_name);
      working = working->_next;
    }
  }
  return result;
}

vector<string> FileTree::getExChildren()
{
  vector<string> result;
  result.insert(result.begin(), "a");
  result.insert(result.begin(), "b");
  result.insert(result.begin(), "c");
  return result;
}
