#ifndef _DOOPEY_FILE_TREE_H_
#define _DOOPEY_FILE_TREE_H_

//#include <ext/hash_map>
#include <string>
#include <functional>

using std::string;
using std::hash;

namespace Doopey{

  template<typename KEY, typename VALUE>
  class HashList
  {
    struct HashItem
    {
       KEY key;
       VALUE value;
       HashItem* next;
    };

    public:
      HashList(uint64_t size = 3000);
      bool add(const KEY& key, const VALUE& value);
      bool remove(const KEY& key);
      VALUE getValue(const KEY& key);
    
    private:
      hash<KEY> keyHash;

      HashItem** _list;
      const uint64_t _listSize;
  };//Class HashList

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
      HashList<string, TreeNode*> _fileTable;
  };//class FileTree


}//namespace Doopey

#endif
