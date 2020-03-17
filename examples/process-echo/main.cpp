#include "process/process.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

int main(int argc, char **argv) {
  std::vector<char> sendbuf(1024 * 16, 'a');
  std::vector<char> recvbuf(1024 * 16, '\0');
  for (int i = 0; i < 10000; ++i) {
    Process p{argv[1]};
    assert(p.isReadable());
    p.writeExact(sendbuf.data(), sendbuf.size());
    p.closeStdin();
    p.readExact(&recvbuf[0], sendbuf.size());
    assert(recvbuf == sendbuf);
    std::transform(recvbuf.begin(), recvbuf.end(), recvbuf.begin(),
                   [](auto a) { return '\0'; });
  }
  {
    Process p{argv[1]};
    p.close();
    assert(!p.isReadable());
  }
}
