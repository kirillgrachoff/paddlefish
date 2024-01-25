#pragma once

#include <coroutine>
#include <iostream>

#include <paddlefish/future/future.hpp>

namespace paddlefish::runtime {

void suspend(std::coroutine_handle<>);

std::optional<std::coroutine_handle<>> take();

template <class T, class Alloc>
void block_on(Future<T, Alloc> f) {
  suspend(std::move(f).into_handle());
  for (uint8_t iteration = 0; iteration < 100; ++iteration) {
    std::cerr << "iteration " << static_cast<int>(iteration) << std::endl;
    auto handle = take();
    if (!handle.has_value()) {
      return;
    }
    handle.value().resume();
  }
}

}  // namespace paddlefish::runtime
