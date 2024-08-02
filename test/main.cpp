#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <paddlefish/future.hpp>
#include <paddlefish/runtime.hpp>
#include <paddlefish/future/when_all.hpp>

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

Future<> loop(auto id, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    std::cerr << "loop { id " << id << " } { i " << i << " } log" << std::endl;
    co_await runtime::sched_yield();
  }
  co_return {};
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

Task parallel() {
  auto [a_r, b_r] = co_await when_all(calculate(10), calculate(20));
  std::cerr << std::endl;
  std::cerr << "10 + 20 = " << a_r + b_r << std::endl;
}

Task parallel_loop() {
  std::tie(std::ignore, std::ignore) = co_await when_all(loop(10, 30), loop(20, 20));
}

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
  run_test(parallel());
  run_test(parallel_loop());
}
