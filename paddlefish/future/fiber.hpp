#pragma once

#include <coroutine>
#include <exception>
#include <optional>

namespace paddlefish {

template <class Future, class Alloc>
class Fiber {
  using FinalSuspendAwaiter = typename Future::FinalSuspendAwaiter;

 public:
  Fiber() = default;
  Fiber(Fiber&&) = default;
  Fiber& operator=(Fiber&&) = default;

  Fiber(const Fiber&) = delete;
  Fiber& operator=(const Fiber&) = delete;

  auto initial_suspend() noexcept {
    return std::suspend_always{};
  }

  auto final_suspend() noexcept {
    return final_suspend_awaiter_;
  }

  void return_void() noexcept {
  }

  void unhandled_exception() noexcept {
    ex_.emplace(std::current_exception());
  }

  Future get_return_object() {
    return Future::from_promise(*this);
  }

  void set_final_suspend_awaiter(FinalSuspendAwaiter awaiter) noexcept {
    final_suspend_awaiter_ = std::move(awaiter);
  }

  void get_void() && {
    if (ex_.has_value()) {
      std::rethrow_exception(*ex_);
    }
  }

 private:
  std::optional<std::exception_ptr> ex_;

  FinalSuspendAwaiter final_suspend_awaiter_;
};

}  // namespace paddlefish
