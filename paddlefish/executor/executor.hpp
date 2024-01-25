#pragma once

#include <coroutine>
#include <optional>

namespace paddlefish::runtime {

void schedule(std::coroutine_handle<>);

std::optional<std::coroutine_handle<>> take();

std::coroutine_handle<> substitute(std::coroutine_handle<> handle);

void utilize();

namespace detail {

struct YieldAwaiter {
  constexpr bool await_ready() noexcept {
    return false;
  }

  auto await_suspend(std::coroutine_handle<> handle) {
    return runtime::substitute(handle);
  }

  void await_resume() noexcept {
  }
};

}  // namespace detail

}  // namespace paddlefish::runtime
