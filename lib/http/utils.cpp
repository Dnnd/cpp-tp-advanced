#include "http/utils.hpp"


JThread::JThread(std::thread &&otherTh) : t(std::move(otherTh)) {}
JThread::~JThread() {
  if (t.joinable()) {
    t.join();
  }
}
