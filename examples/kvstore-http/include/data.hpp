#ifndef PROCESS_WRAPPER_EXAMPLES_KVSTORE_HTTP_DATA_HPP_
#define PROCESS_WRAPPER_EXAMPLES_KVSTORE_HTTP_DATA_HPP_
#include <cstdint>

using Key = uint64_t;
struct Data {
  Key key{0};
  uint8_t data[20]{0};
};

#endif // PROCESS_WRAPPER_EXAMPLES_KVSTORE_HTTP_DATA_HPP_
