#include "cli.hpp"
#include <iostream>
#include <tcp/server.hpp>
#include <vector>

int main(int argc, char **argv) {
  Config cfg = parse_cli_opts(argc, argv);
  tcp::Server server{cfg.hostname, cfg.port};
  tcp::Connection con{server.accept()};
  std::vector<char> buff(257, '\0');
  while (server.isOpened()) {
    std::size_t got = con.read(buff.data(), buff.size() - 1);
    if (got == 0) {
      con = server.accept();
    }
    std::cerr << "Read: " << buff.data() << '\n';
    con.writeExact(buff.data(), got);
  }
}