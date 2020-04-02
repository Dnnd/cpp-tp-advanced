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
  constexpr option hostname = {.name = "hostname",
                               .has_arg = optional_argument,
                               .flag = nullptr,
                               .val = 'h'};
  constexpr option port = {.name = "port",
                           .has_arg = optional_argument,
                           .flag = nullptr,
                           .val = 'p'};

  constexpr option long_options[] = {hostname, port, {nullptr, 0, nullptr, 0}};
  int option_index = 0;
  while (true) {
    int opt = getopt_long(argc, argv, "h:p:", long_options, &option_index);
    if (opt == -1) {
      break;
    }
    switch (opt) {
    case hostname.val:
      parsed_config.hostname = optarg;
      break;
    case port.val:
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
