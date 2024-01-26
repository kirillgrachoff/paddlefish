#pragma once

#include "api.hpp"

#include <paddlefish/future/task.hpp>
#include <paddlefish/future/future.hpp>

namespace paddlefish::runtime {

template <class Alloc>
void go(TaskAlloc<Alloc> f) {
  schedule(std::move(f).into_handle());
}

template <class Alloc>
void go(Future<Alloc> f) {
  schedule(std::move(f).into_handle());
}

template <class F>
void block_on(F&& f) {
  go(std::forward<F>(f));
  utilize();
}

namespace detail {

struct YieldAwaiter {
  constexpr bool await_ready() noexcept {
    return false;
  }

  auto await_suspend(std::coroutine_handle<> handle) {
    return runtime::maybe_schedule(handle);
  }

  void await_resume() noexcept {
  }
};

}  // namespace detail

}  // namespace paddlefish::runtime
