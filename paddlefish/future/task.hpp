#pragma once

#include <cstddef>

#include <paddlefish/executor/executor.hpp>
#include <paddlefish/future/fiber.hpp>
#include <paddlefish/future/final_suspend_awaiter.hpp>

namespace paddlefish {

template <class Alloc = std::allocator<std::byte>>
class TaskAlloc {
 public:
  using PromiseType = Fiber<TaskAlloc<Alloc>, Alloc>;

 public:
  using FinalSuspendAwaiter = detail::FinalSuspendAwaiter;

 private:
  explicit TaskAlloc(PromiseType& promise) : promise_(promise) {
  }

 public:
  TaskAlloc() = delete;
  TaskAlloc(const TaskAlloc&) = delete;
  TaskAlloc& operator=(const TaskAlloc&) = delete;

  TaskAlloc(TaskAlloc&&) = default;
  TaskAlloc& operator=(TaskAlloc&&) = default;

  static TaskAlloc from_promise(PromiseType& promise) {
    return TaskAlloc(promise);
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

  void await_resume() {
    std::move(promise_).get_void();
  }

  decltype(auto) into_handle() && {
    return std::coroutine_handle<PromiseType>::from_promise(promise_);
  }

 private:
  PromiseType& promise_;
};

using Task = TaskAlloc<>;

}  // namespace paddlefish

namespace std {

template <class Alloc, class... Args>
struct coroutine_traits<paddlefish::TaskAlloc<Alloc>, Args...> {
  // NOLINTNEXTLINE
  using promise_type = typename paddlefish::TaskAlloc<Alloc>::PromiseType;
};

}  // namespace std
