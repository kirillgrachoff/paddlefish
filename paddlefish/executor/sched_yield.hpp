#pragma once

#include <paddlefish/runtime.hpp>

namespace paddlefish {

auto sched_yield() {
  return runtime::detail::YieldAwaiter{};
}

}  // namespace paddlefish
