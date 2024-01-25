#include "scheduler.hpp"

#include <iostream>

namespace paddlefish::runtime {

static thread_local FifoScheduler scheduler;

void suspend(std::coroutine_handle<> handle) {
  scheduler.submit(handle);
}

std::optional<std::coroutine_handle<>> take() {
  return scheduler.take();
}

std::coroutine_handle<> substitute(std::coroutine_handle<> handle) {
  return scheduler.substitute(handle);
}

void utilize() {
  for (uint8_t iteration = 0; true; ++iteration) {
    if (iteration == 0) {
      std::cerr << "Runtime iteration" << std::endl;
    }
    auto handle = take();
    if (!handle.has_value()) {
      return;
    }
    handle.value().resume();
  }
}

}  // namespace paddlefish::runtime
