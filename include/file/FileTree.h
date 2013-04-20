#ifndef _DOOPEY_FILE_TREE_H_
#define _DOOPEY_FILE_TREE_H_

#include <string>
#include <functional>
#include <common/Doopey.h>

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
      HashList(uint64_t size = 3000): _listSize(size)
      {
        _list = new HashItem*[size];
        for(uint64_t i=0; i<size; i++)
          _list[i] = NULL;
      }
      ~HashList()
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
      bool add(const KEY& key, const VALUE& value)
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
      bool remove(const KEY& key)
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
      VALUE getValue(const KEY& key)
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
    private:
      hash<KEY> keyHash;

      HashItem** _list;
      const uint64_t _listSize;
  };//Class HashList

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
      BlockID _metaID;

    friend class FileTree;
  };//class TreeNode

  class FileTree{
    public:
      ~FileTree();

      bool addFile(const string& path, const BlockID metaID);
      bool addDir(const string& path);
      bool removeFile(const string& path);
      bool removeDir(const string& path);
      BlockID getMetaID(const string& filePath);
    private:
      FileTree();

      string getFileName(const string& path) const;
      string getFirst(string& s);
      bool removeNode(TreeNode* tn);
      TreeNode* searchDir(const string& dirPath);
      TreeNode* searchChild(TreeNode* node, const string& name) const;

      TreeNode* _root;
      HashList<string, TreeNode*>* _fileMap;
  };//class FileTree

}//namespace Doopey

#endif
