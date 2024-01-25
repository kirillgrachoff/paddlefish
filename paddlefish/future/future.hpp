#pragma once

#include <coroutine>
#include <type_traits>

#include <paddlefish/unit.hpp>

namespace paddlefish {

template <class T, class Alloc = std::allocator<std::byte>>
class Promise;

namespace detail {

template <class T = Unit, class Alloc = std::allocator<std::byte>>
auto make_future(Promise<T, Alloc>& promise) {
  return Future(promise);
}

}  // namespace detail

template <class T = Unit, class Alloc = std::allocator<std::byte>>
class Future {
 public:
  static Future from_promise(Promise<T, Alloc>& promise) {
    Future result;
    result.promise_ = &promise;
    return result;
  }

  void run() {
    while (true) {
      std::coroutine_handle<> handle =
          std::coroutine_handle<decltype(*promise_)>::from_promise(*promise_);
      if (handle.done()) {
        return;
      }
      handle.resume();
    }
  }

 public:
  Promise<T, Alloc>* promise_ = nullptr;
};

}  // namespace paddlefish

namespace std {

template <class T, class Alloc, class... Args>
struct coroutine_traits<paddlefish::Future<T, Alloc>, Args...> {
  // NOLINTNEXTLINE
  using promise_type = paddlefish::Promise<T, Alloc>;
};

}  // namespace std
