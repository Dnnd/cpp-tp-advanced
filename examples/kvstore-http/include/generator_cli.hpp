#ifndef PROCESS_WRAPPER_EXAMPLES_HTTP_EXAMPLE_CLI_HPP_
#define PROCESS_WRAPPER_EXAMPLES_HTTP_EXAMPLE_CLI_HPP_
#include "log/level.hpp"
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <getopt.h>
#include <optional>
#include <string>

struct GeneratorConfig {
  std::string filename{"/tmp/data.db"};
  std::size_t keys_number{65536};
  std::size_t keys_step{100};
};

GeneratorConfig parse_cli_opts(int argc, char *const argv[]) {
  GeneratorConfig parsed_config{};
  while (true) {
    int opt = getopt(argc, argv, "k:n:s:");
    if (opt == -1) {
      break;
    }
    switch (opt) {
    case 'n':
      parsed_config.keys_number = std::stoull(optarg);
      break;
    case 's':
      parsed_config.keys_step = std::stoull(optarg);
      break;
    case 'k':
      parsed_config.filename = optarg;
      break;
    case '?':
    default:
      break;
    }
  }
  return parsed_config;
}
#endif // PROCESS_WRAPPER_EXAMPLES_HTTP_EXAMPLE_CLI_HPP_
