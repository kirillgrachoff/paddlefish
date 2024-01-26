#pragma once

#include <coroutine>
#include <optional>

namespace paddlefish::runtime {

void schedule(std::coroutine_handle<>);

std::optional<std::coroutine_handle<>> take();

std::coroutine_handle<> maybe_schedule(std::coroutine_handle<> handle);

void utilize();

}  // namespace paddlefish::runtime
