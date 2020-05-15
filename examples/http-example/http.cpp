#include "cli.hpp"
#include "http/http_server.hpp"
#include "http/utils.hpp"
#include <iostream>

#include "http/protocol.hpp"
#include "log/thread_safe_logger.hpp"

class HttpEchoServer : public HttpServer {
public:
  HttpEchoServer(const std::string &hostname, uint16_t port,
                 size_t thread_pool_size, std::chrono::milliseconds timeout,
                 std::unique_ptr<log::BaseLogger> logger_)
      : HttpServer{hostname, port, thread_pool_size, timeout,
                   std::move(logger_)} {}
  HttpResponse serveRequest(HttpRequest &request) override {
    std::ostringstream req;
    req << PrettyRequest{request};
    return HttpResponse()
        .addHeader("Content-Type", "text/plain")
        .setStatusCode(HttpStatusCode::OK)
        .setBody(req.str());
  }
};

int main(int argc, char **argv) {
  Config cfg = parse_cli_opts(argc, argv);
  HttpServer::enableGracefulShutdown();
  std::unique_ptr<log::BaseLogger> logger;
  if (cfg.logfile) {
    logger = std::make_unique<log::ThreadSafeFileLogger>(cfg.logfile.value());
  } else {
    logger = std::make_unique<log::ThreadSafeStdErrLogger>();
  }
  logger->set_level(cfg.log_level);
  auto echo_server = std::make_unique<HttpEchoServer>(
      cfg.hostname, cfg.port, cfg.concurrency, cfg.timeout, std::move(logger));
  echo_server->run();
}