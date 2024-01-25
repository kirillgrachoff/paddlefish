#pragma once

#include "executor.hpp"

#include <paddlefish/future/future.hpp>

namespace paddlefish::runtime {

template <class Alloc>
void go(Future<Unit, Alloc> f) {
  schedule(std::move(f).into_handle());
}

template <class Alloc>
void block_on(Future<Unit, Alloc> f) {
  go(std::move(f));
  utilize();
}

}  // namespace paddlefish::runtime
