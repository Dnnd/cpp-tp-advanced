#include "cli.hpp"
#include "tcp/async_connection.hpp"
#include "tcp/async_server.hpp"
#include <cstdio>
#include <memory>
#include <unordered_map>

struct Context {
  std::string read_buffer = std::string(256, '\0');
  std::string response_buffer = {};
  std::size_t read_pos = 0;
  std::size_t write_pos = 0;
};

class Handler {
public:
  using AsyncConnectionPtr = std::shared_ptr<tcp::AsyncConnection>;
  void operator()(const AsyncConnectionPtr &con, EventsSet events) {
    Context &ctx = contexes_registry_[con];
    if (events.contains(Events::ERR)) {
      closeConnection(con);
      return;
    }
    if (events.contains(Events::IN)) {
      handleRead(con, ctx);
    }
    if (events.contains(Events::OUT)) {
      handleWrite(con, ctx);
    }
    if (events.diff({Events ::IN, Events ::OUT}).empty()) {
      closeConnection(con);
    }
  }

private:
  void handleRead(const AsyncConnectionPtr &con, Context &ctx) {
    while (true) {
      tcp::NonBlockResult res =
          con->read(ctx.read_buffer.data() + ctx.read_pos,
                    ctx.read_buffer.size() - ctx.read_pos);
      if (res.isEOF() || res.again()) {
        break;
      }
      if (res.again()) {
        break;
      }
      ctx.read_pos += res.bytes();
      if (ctx.read_buffer.find("\r\n\r\n") != std::string::npos) {
        EventsSet &interests = con->getInterestSet();
        interests.sub(Events::IN);
        interests.add(Events::OUT);
        ctx.response_buffer =
            "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length:0\r\n";
        break;
      }
      if (ctx.read_buffer.size() == ctx.read_pos) {
        ctx.read_buffer.resize(ctx.read_buffer.size() * 2);
      }
    }
  }

  void handleWrite(const AsyncConnectionPtr &con, Context &ctx) {
    while (true) {
      tcp::NonBlockResult res =
          con->write(ctx.response_buffer.data() + ctx.write_pos,
                     ctx.response_buffer.size() - ctx.write_pos);
      if (res.again()) {
        break;
      }
      ctx.write_pos += res.bytes();
      if (ctx.write_pos == ctx.response_buffer.size()) {
        contexes_registry_.erase(con);
        con->close();
        return;
      }
    }
  }

  void closeConnection(const AsyncConnectionPtr &con) {
    contexes_registry_.erase(con);
    con->close();
  }

private:
  std::unordered_map<AsyncConnectionPtr, Context> contexes_registry_ = {};
};

int main(int argc, char **argv) {
  Config cfg = parse_cli_opts(argc, argv);
  tcp::AsyncServer server{cfg.hostname, cfg.port, Handler{}, {Events ::IN}, -1};
  server.accept();
}
