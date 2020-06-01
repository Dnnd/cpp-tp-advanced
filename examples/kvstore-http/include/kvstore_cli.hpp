#ifndef PROCESS_WRAPPER_EXAMPLES_KVSTORE_HTTP_INCLUDE_KVSTORE_CLI_HPP_
#define PROCESS_WRAPPER_EXAMPLES_KVSTORE_HTTP_INCLUDE_KVSTORE_CLI_HPP_
#include "log/level.hpp"
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <getopt.h>
#include <optional>
#include <string>

struct KVStoreConfig {
  std::string hostname = "localhost";
  std::uint16_t port = 8080;
  std::chrono::milliseconds timeout{1000};
  unsigned concurrency{4};
  log::Level log_level = log::Level::INFO;
  std::optional<std::string> logfile = std::nullopt;
  std::string kvstore_file{"/tmp/data.db"};
  std::size_t kvstore_size{0};
};

KVStoreConfig parse_cli_opts(int argc, char *const argv[]) {
  KVStoreConfig parsed_config{};
  while (true) {
    int opt = getopt(argc, argv, "h:p:t:i:w:e:l:s:k:");
    if (opt == -1) {
      break;
    }
    switch (opt) {
    case 'h':
      parsed_config.hostname = optarg;
      break;
    case 'p':
      parsed_config.port = std::stoul(optarg);
      break;
    case 't':
      parsed_config.timeout = std::chrono::milliseconds(std::stoul(optarg));
      break;
    case 'c':
      parsed_config.concurrency = std::stoul(optarg);
      break;
    case 'i':
      parsed_config.log_level = log::Level::INFO;
      break;
    case 'w':
      parsed_config.log_level = log::Level::WARN;
      break;
    case 'e':
      parsed_config.log_level = log::Level::ERROR;
      break;
    case 'd':
      parsed_config.log_level = log::Level::DEBUG;
      break;
    case 'l':
      parsed_config.logfile.emplace(optarg);
      break;
    case 's':
      parsed_config.kvstore_size = std::stoull(optarg);
      break;
    case 'k':
      parsed_config.kvstore_file = optarg;
      break;
    case '?':
    default:
      break;
    }
  }
  return parsed_config;
}
#endif // PROCESS_WRAPPER_EXAMPLES_KVSTORE_HTTP_INCLUDE_KVSTORE_CLI_HPP_
