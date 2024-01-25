#pragma once

#include <paddlefish/runtime.hpp>

namespace paddlefish::runtime {

auto sched_yield() {
  return runtime::detail::YieldAwaiter{};
}

}  // namespace paddlefish::runtime
