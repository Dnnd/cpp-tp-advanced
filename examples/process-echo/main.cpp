#include "process/process.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "usage: process-echo <path_to_executable>\n";
    return EXIT_FAILURE;
  }
  constexpr std::size_t size_16kb = 1024 * 16;

  std::vector<char> sendbuf(size_16kb, 'a');
  std::vector<char> recvbuf(size_16kb, '\0');
  for (int i = 0; i < 10000; ++i) {
    Process p{argv[1]};
    assert(p.isReadable());
    p.writeExact(sendbuf.data(), sendbuf.size());
    p.closeStdin();
    p.readExact(recvbuf.data(), sendbuf.size());
    assert(recvbuf == sendbuf);
    std::transform(recvbuf.begin(), recvbuf.end(), recvbuf.begin(),
                   [](auto a) { return '\0'; });
  }
  {
    Process p{argv[1]};
    p.close();
    assert(!p.isReadable());
  }
  return 0;
}
