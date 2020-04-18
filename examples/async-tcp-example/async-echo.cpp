#include "cli.hpp"
#include "tcp/async_connection.hpp"
#include "tcp/async_server.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sys/epoll.h>
#include <unordered_map>

struct Context {
  std::vector<char> read_buffer = std::vector<char>(256, '\0');
  std::size_t read_pos = 0;
  std::size_t write_pos = 0;
};

class Handler {
public:
  using AsyncConnectionPtr = std::shared_ptr<tcp::AsyncConnection>;

  void operator()(const AsyncConnectionPtr &con, EventsSet events) {
    if (events.contains(Events::ERR)) {
      closeConnection(con);
      return;
    }

    Context &ctx = contexes_registry_[con];
    if (events.contains(Events::IN)) {
      handleRead(*con, ctx);
    }

    if (events.contains(Events::OUT)) {
      handleWrite(*con, ctx);
    }
    bool write_finished = ctx.write_pos == ctx.read_pos;
    if (ctx.write_pos == ctx.read_pos) {
      con->getInterestSet().sub(Events::OUT);
    } else if (!write_finished &&
               !con->getInterestSet().contains(Events::OUT)) {
    }
  }

private:
  void closeConnection(const AsyncConnectionPtr &con) {
    contexes_registry_.erase(con);
    con->close();
  }

  void handleRead(tcp::AsyncConnection &con, Context &ctx) {
    while (true) {
      tcp::NonBlockResult res = con.read(ctx.read_buffer.data() + ctx.read_pos,
                                         ctx.read_buffer.size() - ctx.read_pos);
      if (res.isEOF() || res.again()) {
        break;
      }
      ctx.read_pos += res.bytes();
      if (ctx.read_buffer.size() == ctx.read_pos) {
        ctx.read_buffer.resize(ctx.read_buffer.size() * 2);
      }
    }
    if (ctx.write_pos < ctx.read_pos) {
      con.getInterestSet().add(Events::OUT);
    }
  }

  void handleWrite(tcp::AsyncConnection &con, Context &ctx) {
    while (ctx.write_pos < ctx.read_pos) {
      tcp::NonBlockResult res = con.write(
          ctx.read_buffer.data() + ctx.write_pos, ctx.read_pos - ctx.write_pos);
      if (res.again()) {
        break;
      }
      ctx.write_pos += res.bytes();
    }
    if (ctx.write_pos == ctx.read_pos) {
      con.getInterestSet().sub(Events::OUT);
    }
  }

  std::unordered_map<AsyncConnectionPtr, Context> contexes_registry_;
};

int main(int argc, char **argv) {
  Config cfg = parse_cli_opts(argc, argv);
  tcp::AsyncServer server{
      cfg.hostname, cfg.port, Handler{}, {Events ::IN, Events::OUT}};
  server.accept();
}