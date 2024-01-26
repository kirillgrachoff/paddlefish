#pragma once

#include <coroutine>
#include <deque>
#include <optional>

class FifoScheduler {
public:
  void submit(std::coroutine_handle<> handle) {
    q_.push_back(handle);
  }

  std::optional<std::coroutine_handle<>> take() {
    if (q_.empty()) {
      return std::nullopt;
    } else {
      auto result = std::move(q_.front());
      q_.pop_front();
      return result;
    }
  }

  std::coroutine_handle<> substitute(std::coroutine_handle<> handle) {
    if (q_.empty()) {
      return handle;
    }
    submit(handle);
    return *take();
  }

private:
  std::deque<std::coroutine_handle<>> q_;
};
