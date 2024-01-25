#pragma once

#include <coroutine>
#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <optional>

#include <paddlefish/unit.hpp>
#include <type_traits>

namespace paddlefish {

class PromiseValueIsNotSetError : public std::exception {
  const char* what() const noexcept override {
    return "Promise::get_value() called; value is not set";
  }
};

template <class T, class Future, class Alloc>
class Promise {
  using FinalSuspendAwaiter = typename Future::FinalSuspendAwaiter;

 public:
  Promise() = default;
  Promise(Promise&&) = default;
  Promise& operator=(Promise&&) = default;

  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;

  auto initial_suspend() noexcept {
    return std::suspend_always{};
  }

  auto final_suspend() noexcept {
    return final_suspend_awaiter_;
  }

  void return_value(T value) {
    value_.template emplace<kValue>(std::move(value));
  }

  Future get_return_object() {
    return Future::from_promise(*this);
  }

  void unhandled_exception() {
    auto ptr = std::current_exception();
    value_.template emplace<kError>(Error{ptr});
  }

  void set_final_suspend_awaiter(FinalSuspendAwaiter awaiter) noexcept {
    final_suspend_awaiter_ = std::move(awaiter);
  }

  T get_value() && {
    if (value_.index() == kEmpty) {
      throw PromiseValueIsNotSetError();
    }
    if (value_.index() == kError) {
      std::rethrow_exception(std::get<kError>(value_).ptr);
    }
    return std::get<kValue>(std::move(value_));
  }

 private:
  FinalSuspendAwaiter final_suspend_awaiter_;

  struct Error {
    std::exception_ptr ptr;
  };

  constexpr static size_t kEmpty = 0;
  constexpr static size_t kValue = 1;
  constexpr static size_t kError = 2;
  std::variant<Unit, T, Error> value_;
};

}  // namespace paddlefish
