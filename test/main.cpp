#include <iostream>

#include <paddlefish/future.hpp>

paddlefish::Future<int> Calculate(int value) {
  std::cout << "Calculating " << value;
  co_return value;
}

paddlefish::Future<> Main() {
  auto f = Calculate(20);
  std::cout << "co_await... ";
  int v = co_await f;
  std::cout << " = " << v << std::endl;
  std::cout << "co_await... ";
  int vv = co_await Calculate(40);
  std::cout << " = " << vv << std::endl;
  std::cout << "Resumed successfully" << std::endl;
  co_return {};
}

int main() {
  auto f = Main();
  std::cout << "Run" << std::endl;
  f.run();
  std::cout << "End" << std::endl;
}