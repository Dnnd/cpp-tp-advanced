#ifndef PROCESS_WRAPPER_INCLUDE_HTTP_UTILS_HPP_
#define PROCESS_WRAPPER_INCLUDE_HTTP_UTILS_HPP_

#include "http_request.hpp"
#include "protocol.hpp"
#include <thread>

struct PrettyRequestInfo {
  const HttpRequest &request;
  friend std::ostream &operator<<(std::ostream &os,
                                  const PrettyRequestInfo &p) {
    const HttpRequest &req = p.request;
    os << "[Version]: " << req.getVersion() << "\n";
    os << "[Method]: " << req.getMethod() << "\n";
    os << "[Resource Target]:" << req.getResourceTarget() << "\n";
    os << "[Query Parameters]\n";
    for (auto &&[param_name, param] : req.getQueryParameters()) {
      os << param_name << ": " << param << "\n";
    }
    os << "[Headers]\n";
    for (auto &&[header_name, header_value] : req.getHeadres()) {
      os << header_name << ": " << header_value << "\n";
    }
    return os;
  }
};

struct PrettyRequest {
  const HttpRequest &request;
  friend std::ostream &operator<<(std::ostream &os, const PrettyRequest &p) {
    os << PrettyRequestInfo{p.request};
    if (!p.request.getBody().empty()) {
      os << "[Body]\n";
      os << p.request.getBody();
    }
    return os;
  }
};

class JThread {
public:
  JThread(std::thread &&otherTh);

  ~JThread();

  JThread(JThread &&) = default;
  JThread &operator=(JThread &&) = default;

private:
  std::thread t;
};

#endif // PROCESS_WRAPPER_INCLUDE_HTTP_UTILS_HPP_
