#include "api.hpp"

#include "fifo_scheduler.hpp"

namespace paddlefish::runtime {

namespace {

thread_local FifoScheduler scheduler;
thread_local size_t iteration = 0;

constexpr size_t kFifo = 17;
constexpr size_t kIO = 63;

}  // namespace

void schedule(std::coroutine_handle<> handle) {
  scheduler.submit(handle);
}

std::optional<std::coroutine_handle<>> take() {
  return scheduler.take();
}

std::coroutine_handle<> maybe_schedule(std::coroutine_handle<> handle) {
  if (iteration++ % kFifo != 0) {
    return handle;
  }
  return scheduler.substitute(handle);
}

void utilize() {
  for (uint8_t iteration = 0; true; ++iteration) {
    auto handle = take();
    if (!handle.has_value()) {
      return;
    }
    handle.value().resume();
  }
}

}  // namespace paddlefish::runtime
