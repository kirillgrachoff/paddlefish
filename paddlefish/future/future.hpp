#pragma once

#include <coroutine>
#include <type_traits>
#include <iostream>

#include <paddlefish/executor/executor.hpp>
#include <paddlefish/unit.hpp>

namespace paddlefish {

template <class T, class Future, class Alloc>
class Promise;

template <class T = Unit, class Alloc = std::allocator<std::byte>>
class Future {
 public:
  // NOLINTNEXTLINE
  using promise_type = Promise<T, Future<T, Alloc>, Alloc>;

 public:
  struct FinalSuspendAwaiter {
    std::false_type await_ready() noexcept {
      return {};
    }

    auto await_suspend(std::coroutine_handle<> handle) noexcept {
      // runtime::schedule(caller);
      return caller;
    }

    void await_resume() noexcept {
    }

    std::coroutine_handle<> caller;
  };

 private:
  explicit Future(promise_type& promise)
    : promise_(promise)
  {}

 public:
  Future() = delete;
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  Future(Future&&) = default;
  Future& operator=(Future&&) = default;

  static Future from_promise(promise_type& promise) {
    return Future(promise);
  }

  std::false_type await_ready() noexcept {
    return {};
  }

  auto await_suspend(std::coroutine_handle<> handle) {
    promise_.set_final_suspend_awaiter(FinalSuspendAwaiter{handle});
    auto promise_handle = std::coroutine_handle<decltype(promise_)>::from_promise(promise_);
    return paddlefish::runtime::maybe_schedule(promise_handle);
  }

  T await_resume() {
    return std::move(promise_).get_value();
  }

  decltype(auto) into_handle() && {
    promise_.set_final_suspend_awaiter(FinalSuspendAwaiter{std::noop_coroutine()});
    return std::coroutine_handle<std::remove_reference_t<decltype(promise_)>>::from_promise(promise_);
  }

 private:
  promise_type& promise_;
};

template <class Alloc = std::allocator<std::byte>>
using Task = Future<Unit, Alloc>;

}  // namespace paddlefish

namespace std {

template <class T, class Alloc, class... Args>
struct coroutine_traits<paddlefish::Future<T, Alloc>, Args...> {
  // NOLINTNEXTLINE
  using promise_type = typename paddlefish::Future<T, Alloc>::promise_type;
};

}
