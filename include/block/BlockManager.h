#ifndef _DOOPEY_BLOCK_MANAGER_H_
#define _DOOPEY_BLOCK_MANAGER_H_

namespace Doopey {

  class Router;
  class BlockLoader;
  class BlockResolver;
  class BlockSaver;
  class BlockUpdater;

  class BlockManager {
    public:
      BlockManager(Router* router);
      ~BlockManager();

      // request handle interface
      // ex. getBlock

    private:
      Router* _router;

    private: // owner
      BlockLoader* _loader;
      BlockResolver* _resolver;
      BlockSaver* _saver;
      BlockUpdater* _updater;

  }; // class BlockManager

};  // namespace Doopey

#endif
