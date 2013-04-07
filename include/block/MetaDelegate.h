#ifndef _DOOPEY_META_DELEGATE_H_
#define _DOOPEY_META_DELEGATE_H_

#include "block/OperationDelegate.h"

namespace Doopey {

  class MetaDelegate: public OperationDelegate {
    public:
      MetaDelegate();
      virtual ~MetaDelegate() {}
  }; // class MetaDelegate

};  // namespace Doopey

#endif
