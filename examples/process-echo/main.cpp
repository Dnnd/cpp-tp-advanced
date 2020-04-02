#include "process/process.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "usage: process-echo <path_to_executable>\n";
    return EXIT_FAILURE;
  }
  constexpr std::size_t test_buff_size = 1024 * 16;

  std::vector<char> sendbuf(test_buff_size, 'a');
  std::vector<char> recvbuf(test_buff_size, '\0');
  for (int i = 0; i < 100; ++i) {
    Process p{argv[1]};
    assert(p.isReadable());
    p.writeExact(sendbuf.data(), sendbuf.size());
    p.closeStdin();
    p.readExact(recvbuf.data(), sendbuf.size());
    assert(recvbuf == sendbuf);
    std::transform(recvbuf.begin(), recvbuf.end(), recvbuf.begin(),
                   [](char a) { return '\0'; });
  }
  {
    Process p{argv[1]};
    p.close();
    assert(!p.isReadable());
  }
  return 0;
}
