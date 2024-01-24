#pragma once

#include <__coroutine/noop_coroutine_handle.h>
#include <__coroutine/trivial_awaitables.h>
#include <coroutine>

#include <paddlefish/unit.hpp>

namespace paddlefish {

template <class T = Unit>
class Future {
  struct FutureAwaiter {

  };

  class Promise {
    auto initial_suspend() {
      return std::suspend_never{};
    }

    auto final_suspend() {
      return std::suspend_always{};
    }
  };
 public:
  auto operator co_await() {
    return FutureAwaiter{};
  }
};

} // namespace paddlefish
