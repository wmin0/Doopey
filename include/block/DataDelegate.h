#ifndef _DOOPEY_DATA_DELEGATE_H_
#define _DOOPEY_DATA_DELEGATE_H_

#include "block/OperationDelegate.h"

namespace Doopey {

  class DataDelegate: public OperationDelegate {
    public:
      DataDelegate();
      virtual ~DataDelegate() {}
  }; // class DataDelegate

};  // namespace Doopey

#endif
