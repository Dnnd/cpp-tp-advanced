#ifndef PROCESS_WRAPPER_EXAMPLES_TCP_EXAMPLE_CLI_HPP_
#define PROCESS_WRAPPER_EXAMPLES_TCP_EXAMPLE_CLI_HPP_
#include <cstdint>
#include <cstdlib>
#include <getopt.h>
#include <string>

struct Config {
  std::string hostname = "localhost";
  std::uint16_t port = 8080;
};

Config parse_cli_opts(int argc, char *const argv[]) {
  Config parsed_config{};
  while (true) {
    int opt = getopt(argc, argv, "h:p:");
    if (opt == -1) {
      break;
    }
    switch (opt) {
    case 'h':
      parsed_config.hostname = optarg;
      break;
    case 'p':
      parsed_config.port = std::strtol(optarg, nullptr, 10);
      break;
    case '?':
    default:
      break;
    }
  }
  return parsed_config;
}
#endif // PROCESS_WRAPPER_EXAMPLES_TCP_EXAMPLE_CLI_HPP_
