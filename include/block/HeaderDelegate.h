#ifndef _DOOPEY_HEADERDELEGATE_H_
#define _DOOPEY_HEADERDELEGATE_H_

namespace Doopey {

  class HeaderDelegateRequest {
    HeaderDelegateRequest(const char* f, void* p, size_t s):
      field(f), ptr(p), size(s) {}
    const char* field;
    void* ptr;
    size_t size;
  }; // class HeaderDelegateRequest


  class HeaderDelegate {
    public:
      HeaderDelegate(unsigned char* data): _data(data) {}
      virtual ~HeaderDelegate() {}

      bool read(HeaderDelegateRequest& r) const = 0;
      bool write(HeaderDelegateRequest& r) = 0;

    private:
      const unsigned char* _data;
      // map<string, size_t> _fieldOffsetMap;
  }; // class HeaderDelegate

};  // namespace Doopey

#endif
