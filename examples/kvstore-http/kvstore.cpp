#include "data.hpp"
#include "kvstore/store.hpp"
#include "kvstore_cli.hpp"
#include <charconv>
#include <http/http_server.hpp>
#include <iostream>
#include <log/thread_safe_logger.hpp>

class HttpKVStoreServer : public HttpServer {
public:
  HttpKVStoreServer(const std::string &hostname, uint16_t port,
                    size_t thread_pool_size, std::chrono::milliseconds timeout,
                    std::unique_ptr<log::BaseLogger> logger_,
                    KVStore<Key, Data> store)
      : HttpServer{hostname, port, thread_pool_size, timeout,
                   std::move(logger_)},
        store_{std::move(store)} {}

  HttpResponse serveRequest(HttpRequest &request) override {
    std::optional<std::string_view> key_str = request.getQueryParameter("key");
    if (!key_str) {
      return HttpResponse().setStatusCode(HttpStatusCode::BAD_REQUEST);
    }
    Key key;
    auto [last, ec] = std::from_chars(key_str->begin(), key_str->end(), key);
    if (ec != std::errc()) {
      return HttpResponse().setStatusCode(HttpStatusCode::BAD_REQUEST);
    }
    std::optional<Data> maybe_data = store_.get(key);
    if (!maybe_data) {
      return HttpResponse().setStatusCode(HttpStatusCode::NOT_FOUND);
    }
    Data &data = maybe_data.value();
    std::ostringstream oss;
    oss << R"({"key":)" << data.key << R"(,"value":")";
    for (auto d : data.data) {
      oss << char(d);
    }
    oss << R"("})";
    return HttpResponse()
        .addHeader("Content-Type", "application/json")
        .setStatusCode(HttpStatusCode::OK)
        .setBody(oss.str());
  }

private:
  KVStore<Key, Data> store_;
};

int main(int argc, char **argv) {
  KVStoreConfig cfg = parse_cli_opts(argc, argv);
  std::unique_ptr<log::BaseLogger> logger;
  if (cfg.logfile) {
    logger = std::make_unique<log::ThreadSafeFileLogger>(cfg.logfile.value());
  } else {
    logger = std::make_unique<log::ThreadSafeStdErrLogger>();
  }
  KVStore<Key, Data> kvstore{cfg.kvstore_file, cfg.kvstore_size * sizeof(Data),
                             1024};
  logger->set_level(cfg.log_level);
  HttpKVStoreServer kvserver{cfg.hostname,      cfg.port,
                             cfg.concurrency,   cfg.timeout,
                             std::move(logger), std::move(kvstore)};
  kvserver.run();
}