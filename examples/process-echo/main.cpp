#include "process/process.hpp"
#include <cassert>
#include <vector>

int main() {

  std::vector<char> sendbuf(1024 * 16, 'a');
  std::vector<char> recvbuf(1024 * 16, '\0');
  for (int i = 0; i < 100000; ++i) {
    std::string path = "/home/danm/code/cpp-tp-advanced/cmake-build-release/echo";
    Process p{path};
    p.writeExact(sendbuf.data(), sendbuf.size());
    p.closeStdin();
    p.readExact(&recvbuf[0], sendbuf.size());
    assert(recvbuf == sendbuf);
    sendbuf.clear();
    recvbuf.clear();
  }
}
