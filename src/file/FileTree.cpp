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

TreeNode::~TreeNode()
{
  TreeNode* work = _children;
  TreeNode* next;
  while(work!=NULL)
  {
    next = work->_next;
    delete work;
    work = next;
  }
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
  _fileMap->add("/", _root);
}

FileTree::~FileTree()
{
  log->info("Start release FileTree\n");
  delete _fileMap;
  delete _root;
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
  return _fileMap->getValue(dirPath);
}

bool FileTree::addFile(const string& path, const BlockID metaID)
{
  //check the file if exist or not
  if(_fileMap->getValue(path) != NULL){
    log->info("FileTree: File %s exists\n", path.data());
    return false;
  }

  //path should be definite path
  //aplit the path to dirPath and filename
  string dir;
  if(path.find_last_of("/")==string::npos){
    log->info("FileTree: path %s error\n", path.data());
    return false;
  }else if(path.find_last_of("/")==0){
    dir = "/";
  }else{
    dir = path.substr(0, path.find_last_of("/"));
  }
  string name = getFileName(path);
  TreeNode* insert = searchDir(dir);

  //check the dir if exist or not
  if(insert == NULL){
    log->info("FileTree: dir %s does not exist, please add dir first\n", dir.data());
    return false;
  }

  //check the dirName is really a dir
  if(insert->_isFile == true)
  {
    log->info("FileTree: dir name is a file actually\n");
    return false;
  }

  //used when called by addDir
  if(insert->_children != NULL)
  {
    insert = insert->_children;
    while(insert->_next != NULL)
      insert = insert->_next;
    insert->_next = new TreeNode();
    (insert->_next)->_last = insert;
    insert = insert->_next;
  }else{
  //add file
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

BlockID FileTree::getMetaID(const string& filePath) const
{
  TreeNode* tn = _fileMap->getValue(filePath);
  if(tn == NULL)
    return 0;
  else
    return tn->getID();
}
vector<string> FileTree::getChildren(const string& filePath) const
{
  log->info("FileTree: Request file list of %s\n", filePath.data());
  TreeNode* tn = _fileMap->getValue(filePath);
  vector<string> result;
  string addString;
  if(tn != NULL){
    TreeNode* working = tn->_children;
    while(working!=NULL){
      addString = working->_name;
      if(!working->_isFile)
        addString = addString + "/";
      result.insert(result.begin(), addString);
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
