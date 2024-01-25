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

private:
  std::deque<std::coroutine_handle<>> q_;
};
