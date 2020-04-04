#include "log/logger.hpp"
#include <filesystem>
#include <getopt.h>
#include <optional>

struct Config {
  log::Level log_level = log::Level::INFO;
  std::optional<std::string> logfile = std::nullopt;
};

Config parse_cli_opts(int argc, char *const argv[]) {
  Config parsed_config{};
  constexpr option debug = {
      .name = "debug", .has_arg = no_argument, .flag = 0, .val = 'd'};
  constexpr option info = {
      .name = "info", .has_arg = no_argument, .flag = 0, .val = 'i'};
  constexpr option warn = {
      .name = "warn", .has_arg = no_argument, .flag = 0, .val = 'w'};
  constexpr option error = {
      .name = "error", .has_arg = no_argument, .flag = 0, .val = 'e'};
  constexpr option logfile = {
      .name = "logfile", .has_arg = required_argument, .flag = 0, .val = 'l'};
  constexpr option long_options[] = {debug, info,    warn,
                                     error, logfile, {0, 0, 0, 0}};
  int option_index = 0;
  while (true) {
    int opt = getopt_long(argc, argv, "diwel:", long_options, &option_index);
    if (opt == -1) {
      break;
    }
    switch (opt) {
    case debug.val:
      parsed_config.log_level = log::Level::DEBUG;
      break;
    case error.val:
      parsed_config.log_level = log::Level::ERROR;
      break;
    case warn.val:
      parsed_config.log_level = log::Level::WARN;
      break;
    case info.val:
      parsed_config.log_level = log::Level::INFO;
      break;
    case logfile.val:
      parsed_config.logfile.emplace(optarg);
      break;
    case '?':
    default:
      break;
    }
  }
  return parsed_config;
}

int main(int argc, char **argv) {
  Config config = parse_cli_opts(argc, argv);
  std::unique_ptr<log::BaseLogger> logger;
  if (config.logfile.has_value()) {
    logger = log::create_file_logger(config.logfile.value());
  } else {
    logger = log::create_stdout_logger();
  }
  logger->set_level(config.log_level);
  set_global_logger(std::move(logger));
  log::debug("this is debug message");
  log::info("this is info message");
  log::warn("this is warn message");
  log::error("this is error message");
  return EXIT_SUCCESS;
}