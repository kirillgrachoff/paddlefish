#pragma once

#include <coroutine>
#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <optional>

#include <paddlefish/unit.hpp>
#include <paddlefish/future/future.hpp>

namespace paddlefish {

template <class T = Unit, class Alloc>
class Promise {
public:
  struct FutureAwaiter {
    std::false_type await_ready() noexcept {
      return {};
    }

    auto await_suspend(std::coroutine_handle<> handle) {
      promise.caller = handle;
      return std::coroutine_handle<decltype(promise)>::from_promise(promise);
    }

    T await_resume() {
      return promise.get_value();
    }

    Promise<T, Alloc>& promise;
  };

public:
  auto initial_suspend() noexcept {
    return std::suspend_always{};
  }

  auto final_suspend() noexcept {
    return std::suspend_never{};
  }

  void return_value(T value) {
    value_.template emplace<kValue>(std::move(value));
  }

  Future<T, Alloc> get_return_object() {
    return Future<T, Alloc>::from_promise(*this);
  }

  void unhandled_exception() {
    auto ptr = std::current_exception();
    value_.template emplace<kError>(Error{ptr});
  }

  template <class U, class Alloc2>
  static auto await_transform(const Future<U, Alloc2>& future) {
    return typename Promise<U, Alloc2>::FutureAwaiter{*future.promise_};
  }

  T get_value() {
    if (value_.index() == kEmpty) {
      throw std::bad_exception();
    }
    if (value_.index() == kError) {
      throw std::get<kError>(value_).ptr;
    }
    return std::get<kValue>(value_);
  }

private:
  std::coroutine_handle<> caller = std::noop_coroutine();

  struct Error {
    std::exception_ptr ptr;
  };

  constexpr static size_t kEmpty = 0;
  constexpr static size_t kValue = 1;
  constexpr static size_t kError = 2;
  std::variant<Unit, T, Error> value_;
};

} // namespace paddlefish
