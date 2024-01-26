#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <paddlefish/future.hpp>
#include <paddlefish/runtime.hpp>

using namespace paddlefish;  // NOLINT

Future<int> calculate(int value) {
  std::cerr << "Calculating " << value;
  co_return value;
}

Future<std::unique_ptr<int>> allocate(int value) {
  co_return std::make_unique<int>(value);
}

Task recursive_impl(auto id, int start, int n) {
  if (n == 0) {
    std::cerr << "recursive { id " << id << " } { n " << n << " } end"
              << std::endl;
    co_return;
  }
  std::cerr << "recursive { id " << id << " } { n " << n << " } log"
            << std::endl;
  co_await recursive_impl(id, start, n - 1);
}

Task recursive(int start) {
  auto id = std::rand();
  std::cerr << "recursive { id " << id << " } start" << std::endl;
  co_await recursive_impl(id, start, start);
  std::cerr << "recursive { id " << id << " } end" << std::endl;
}

Task loop(auto id, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    std::cerr << "loop { id " << id << " } { i " << i << " } log" << std::endl;
    co_await runtime::sched_yield();
  }
}

Future<> exceptional_future() {
  throw std::runtime_error("exception from exceptional");

  co_return {};
}

Task exceptional_task() {
  throw std::runtime_error("exception from exceptional");
  co_return;  // This is necessary
}

Future<> noexceptional() {
  try {
    co_await exceptional_task();
  } catch (std::runtime_error& ex) {
    std::cerr << "exception catched :: OK" << std::endl;
  }

  try {
    co_await exceptional_future();
  } catch (std::runtime_error& ex) {
    std::cerr << "exception catched :: OK" << std::endl;
  }

  co_return {};
}

Task check() {
  co_await recursive(1000);
}

Task sequence() {
  auto f = calculate(20);
  std::cerr << "co_await... ";
  int v = co_await f;
  std::cerr << " = " << v << std::endl;
  std::cerr << "co_await... ";
  int vv = co_await calculate(40);
  std::cerr << " = " << vv << std::endl;
  auto vvv = co_await allocate(50);
  std::cerr << *vvv << std::endl;
  std::cerr << "Resumed successfully" << std::endl;
}

Task concurrent() {
  runtime::go(recursive(32));
  runtime::go(recursive(25));
  co_return;
}

// Future<> parallel() {
//   auto a = calculate(10);
//   auto b = calculate(20);
//   auto [a_r, b_r] = when_all(a, b);
// }

void run_test(Task test) {
  static size_t number = 0;
  ++number;
  std::cout << "-- Run test " << number << std::endl;
  runtime::block_on(std::move(test));
  std::cout << "-- End test " << number << std::endl;
}

int main() {
  run_test(concurrent());
  run_test(sequence());
  run_test([]() -> Task {
    runtime::go(loop(1, 50));
    runtime::go(loop(24, 50));
    co_return;
  }());
}
