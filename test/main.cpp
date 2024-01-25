#include <iostream>

#include <paddlefish/future.hpp>
#include <paddlefish/runtime.hpp>
#include "paddlefish/future/future.hpp"

paddlefish::Future<int> calculate(int value) {
  std::cout << "Calculating " << value;
  co_return value;
}

paddlefish::Future<std::unique_ptr<int>> allocate(int value) {
  co_return std::make_unique<int>(value);
}

paddlefish::Future<> recursive(int n = 0) {
  if (n == 1000) {
    co_return {};
  }
  co_await recursive(n + 1);
  co_return {};
}

paddlefish::Future<> sequence() {
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
  co_return {};
}

// paddlefish::Future<> parallel() {
//   auto a = calculate(10);
//   auto b = calculate(20);
//   auto [a_r, b_r] = co_await paddlefish::when_all(a, b);
// }

int main() {
  std::cout << "Run" << std::endl;
  paddlefish::runtime::block_on(sequence());
  std::cout << "End" << std::endl;
}