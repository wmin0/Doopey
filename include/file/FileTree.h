#ifndef _DOOPEY_FILE_TREE_H_
#define _DOOPEY_FILE_TREE_H_

//#include <ext/hash_map>
#include <string>

using std::string;

namespace Dooepy{

  class TreeNode{
    friend class FileTree;

    public:
      ~TreeNode();
    private:
      TreeNode();
      string filename;
      TreeNode* children;
      TreeNode* next;
  };//class TreeNode

  class FileTree{
    public:
      FileTree();
      ~FileTree();
    private:
//      hash_map<string, void*> _fileMap;
  };//class FileTree

}//namespace Doopey

#endif
