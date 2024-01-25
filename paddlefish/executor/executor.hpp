#pragma once

#include <coroutine>
#include <optional>

#include <paddlefish/future/future.hpp>

namespace paddlefish::runtime {

void suspend(std::coroutine_handle<>);

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


template <class Alloc>
void go(Future<Unit, Alloc> f) {
  suspend(std::move(f).into_handle());
}

template <class Alloc>
void block_on(Future<Unit, Alloc> f) {
  go(std::move(f));
  utilize();
}

}  // namespace paddlefish::runtime
