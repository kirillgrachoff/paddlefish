#pragma once

#include <coroutine>
#include <type_traits>

namespace paddlefish {

namespace detail {

struct FinalSuspendAwaiter {
  std::false_type await_ready() noexcept {
    return {};
  }

  auto await_suspend(std::coroutine_handle<> handle) noexcept {
    return caller;
  }

  void await_resume() noexcept {
  }

  std::coroutine_handle<> caller = std::noop_coroutine();
};

}  // namespace detail

}
