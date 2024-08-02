#pragma once

#include "future.hpp"

#include <tuple>

namespace paddlefish {

template <class... Ts, class... Allocs>
Future<std::tuple<Ts...>> when_all(Future<Ts, Allocs>&&... args) {
  co_return {co_await args...};
}

}  // namespace paddlefish
