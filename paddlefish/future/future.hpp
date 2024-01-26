#pragma once

#include <coroutine>
#include <type_traits>
#include <iostream>

#include <paddlefish/executor/api.hpp>
#include <paddlefish/future/final_suspend_awaiter.hpp>
#include <paddlefish/future/promise.hpp>
#include <paddlefish/unit.hpp>

namespace paddlefish {

template <class T = Unit, class Alloc = std::allocator<std::byte>>
class Future {
 public:
  using PromiseType = Promise<T, Future<T, Alloc>, Alloc>;

 public:
  using FinalSuspendAwaiter = detail::FinalSuspendAwaiter;

 private:
  explicit Future(PromiseType& promise) : promise_(promise) {
  }

 public:
  Future() = delete;
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  Future(Future&&) = default;
  Future& operator=(Future&&) = default;

  static Future from_promise(PromiseType& promise) {
    return Future(promise);
  }

  std::false_type await_ready() noexcept {
    return {};
  }

  auto await_suspend(std::coroutine_handle<> handle) {
    promise_.set_final_suspend_awaiter(FinalSuspendAwaiter{handle});
    auto promise_handle =
        std::coroutine_handle<decltype(promise_)>::from_promise(promise_);
    return paddlefish::runtime::maybe_schedule(promise_handle);
  }

  T await_resume() {
    return std::move(promise_).get_value();
  }

  decltype(auto) into_handle() && {
    return std::coroutine_handle<PromiseType>::from_promise(promise_);
  }

 private:
  PromiseType& promise_;
};

}  // namespace paddlefish

namespace std {

template <class T, class Alloc, class... Args>
struct coroutine_traits<paddlefish::Future<T, Alloc>, Args...> {
  // NOLINTNEXTLINE
  using promise_type = typename paddlefish::Future<T, Alloc>::PromiseType;
};

}  // namespace std
