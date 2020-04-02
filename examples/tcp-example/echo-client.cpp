#include "cli.hpp"
#include "tcp/connection.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv) {
  Config cfg = parse_cli_opts(argc, argv);
  tcp::Connection con{cfg.hostname, cfg.port};
  std::string out_buffer;
  std::string in_buffer;
  while (std::cin) {
    std::getline(std::cin, out_buffer);
    con.writeExact(out_buffer.c_str(), out_buffer.size());
    in_buffer.resize(out_buffer.size(), '\0');
    con.readExact(in_buffer.data(), in_buffer.size());
    std::cout << "Read: " << in_buffer << '\n';
  }
  return EXIT_SUCCESS;
}