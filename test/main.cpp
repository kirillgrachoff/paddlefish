#include <iostream>

#include <paddlefish/future.hpp>

paddlefish::Future<int> calculate(int value) {
  std::cout << "Calculating " << value;
  co_return value;
}

paddlefish::Future<std::unique_ptr<int>> allocate(int value) {
  co_return std::make_unique<int>(value);
}

paddlefish::Future<> app_main() {
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
  co_return {};
}

int main() {
  auto ptr = new int(10);
  auto f = app_main();
  std::cout << "Run" << std::endl;
  f.run();
  std::cout << "End" << std::endl;
}