#ifndef _DOOPEY_FILE_TREE_H_
#define _DOOPEY_FILE_TREE_H_

#include <string>
#include <vector>
#include <functional>
#include <common/Doopey.h>


using std::string;
using std::vector;
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
      ~HashList();
      bool add(const KEY& key, const VALUE& value);
      bool remove(const KEY& key);
      VALUE getValue(const KEY& key);
    private:
      hash<KEY> keyHash;

      HashItem** _list;
      const uint64_t _listSize;
  };//Class HashList
  #include "HashList.tpp"

  class TreeNode{
    public:
      ~TreeNode();
      BlockID getID() const;
    private:
      TreeNode();
      bool _isFile;
      string _name;
      TreeNode* _children;
      TreeNode* _next;
      TreeNode* _last;
      TreeNode* _parent;
      BlockID _metaID;

    friend class FileTree;
    friend class FileTreeTest;
  };//class TreeNode

  class FileTree{
    public:
      ~FileTree();
      FileTree();
      bool addFile(const string& path, const BlockID metaID);
      bool addDir(const string& path);
      bool removeFile(const string& path);
      bool removeDir(const string& path);
      BlockID getMetaID(const string& filePath);
      vector<string> getChildren(const string& filePath);
    private:
      string getFileName(const string& path) const;
      string getFirst(string& s);
      bool removeNode(TreeNode* tn);
      TreeNode* searchDir(const string& dirPath);
      TreeNode* searchChild(TreeNode* node, const string& name) const;

      TreeNode* _root;
      HashList<string, TreeNode*>* _fileMap;

    friend class FileTreeTest;
  };//class FileTree

}//namespace Doopey

#endif
