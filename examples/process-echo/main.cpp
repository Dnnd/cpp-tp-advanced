#include <iostream>
#include <vector>
#include "process/process.hpp"

int main() {
    std::string path = "/home/danm/code/process-wrapper/cmake-build-debug/echo";
    Process p{path};
    std::vector<char> sendbuf(1024 * 16, 'a');
    std::vector<char> recvbuf(1024 * 16 + 1, '\0');
    p.writeExact(sendbuf.data(), sendbuf.size());
    p.closeStdin();
    std::cout << p.read(&recvbuf[0], sendbuf.size()) << '\n';
    std::cout << std::string(&recvbuf[0]) << '\n';

}
