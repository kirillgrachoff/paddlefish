#pragma once

#include <coroutine>
#include <iostream>

#include <paddlefish/future/future.hpp>

namespace paddlefish::runtime {

void suspend(std::coroutine_handle<>);

std::optional<std::coroutine_handle<>> take();

std::coroutine_handle<> substitute(std::coroutine_handle<> handle);

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

template <class T, class Alloc>
void block_on(Future<T, Alloc> f) {
  suspend(std::move(f).into_handle());
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
