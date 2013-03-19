#ifndef _DOOPEY_BLOCKRESOLVER_H_
#define _DOOPEY_BLOCKRESOLVER_H_

#include <map>
#include <string>

using std::string;
using std::map;

namespace Doopey {

  class BlockResolver {

    public:
      BlockResolver() {}
      virtual ~BlockResolver() {}

    private:
      map<uint64_t, string> _idNodeMap;      

  }; // class BlockResolver

};  // namespace Doopey

#endif
