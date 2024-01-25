#include "scheduler.hpp"

namespace paddlefish::runtime {

static thread_local FifoScheduler scheduler;

void suspend(std::coroutine_handle<> handle) {
  scheduler.submit(handle);
}

std::optional<std::coroutine_handle<>> take() {
  return scheduler.take();
}

}  // namespace paddlefish::runtime
