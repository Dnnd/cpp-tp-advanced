#include "cli.hpp"
#include <csignal>
#include <cstring>
#include <iostream>
#include <tcp/server.hpp>
#include <vector>

static volatile bool close_server = false;

static void handler(int sig, siginfo_t *si, void *unused) {
  close_server = true;
}

int main(int argc, char **argv) {
  struct sigaction sa {};

  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = handler;
  if (sigaction(SIGINT, &sa, nullptr) == -1) {
    std::cerr << std::strerror(errno) << '\n';
    return EXIT_FAILURE;
  }

  Config cfg = parse_cli_opts(argc, argv);
  tcp::Server server{cfg.hostname, cfg.port};
  tcp::Connection con{server.accept()};
  std::vector<char> buff(257, '\0');
  while (!close_server) {
    int got = con.read(buff.data(), buff.size() - 1);
    if (got == 0) {
      con = tcp::Connection{server.accept()};
    }
    std::cerr << "Read: " << buff.data() << '\n';
    con.writeExact(buff.data(), got);
  }
}