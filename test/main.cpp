#include <iostream>

#include <paddlefish/future.hpp>
#include <paddlefish/runtime.hpp>
#include <stdexcept>

#include <paddlefish/executor/sched_yield.hpp>
#include <paddlefish/future/future.hpp>

paddlefish::Future<int> calculate(int value) {
  std::cout << "Calculating " << value;
  co_return value;
}

paddlefish::Future<std::unique_ptr<int>> allocate(int value) {
  co_return std::make_unique<int>(value);
}

paddlefish::Task recursive(int n = 0) {
  if (n == 1000) {
    std::cout << "recursive: n = " << n << std::endl;
    co_return;
  }
  co_await recursive(n + 1);
}

paddlefish::Task loop() {
  for (size_t i = 0; i < 1000; ++i) {
    if (i % 100 == 0) {
      std::cout << "loop " << i << std::endl;
    }
    co_await paddlefish::runtime::sched_yield();
  }
}

paddlefish::Future<> exceptional_future() {
  throw std::runtime_error("exception from exceptional");

  co_return {};
}

paddlefish::Task exceptional_task() {
  throw std::runtime_error("exception from exceptional");
}

paddlefish::Future<> noexceptional() {
  try {
    co_await exceptional_future();
  } catch (std::runtime_error& ex) {
    std::cout << "exception catched :: OK" << std::endl;
  }

  try {
    co_await exceptional_task();
  } catch (std::runtime_error& ex) {
    std::cout << "exception catched :: OK" << std::endl;
  }

  co_return {};
}

paddlefish::Future<void> check() {
  co_await recursive(1000);
}

paddlefish::Task sequence() {
  auto f = calculate(20);
  std::cout << "co_await... ";
  int v = co_await f;
  std::cout << " = " << v << std::endl;
  std::cout << "co_await... ";
  int vv = co_await calculate(40);
  std::cout << " = " << vv << std::endl;
  auto vvv = co_await allocate(50);
  std::cout << *vvv << std::endl;
  std::cout << "Resumed successfully" << std::endl;
  std::cout << "Recursive start" << std::endl;
  co_await recursive();
  std::cout << "Recursive end" << std::endl;
  std::cout << "Loop start" << std::endl;
  co_await loop();
  std::cout << "Loop end" << std::endl;
  std::cout << "Exceptional start" << std::endl;
  co_await noexceptional();
  std::cout << "Exceptional end" << std::endl;
  co_await check();
}

// paddlefish::Future<> parallel() {
//   auto a = calculate(10);
//   auto b = calculate(20);
//   auto [a_r, b_r] = paddlefish::when_all(a, b);
// }

int main() {
  std::cout << "Run" << std::endl;
  paddlefish::runtime::block_on(sequence());
  std::cout << "End" << std::endl;
}
