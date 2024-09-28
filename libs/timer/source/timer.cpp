#include <robotics/timer/timer.hpp>

#if defined(__TEST_ON_HOST__)
#include "timer.mock.hpp"
#elif defined(__MBED__)
#include "timer.mbed.hpp"
#elif defined(ESP_PLATFORM)
#include "timer.idf.hpp"
#else
#include "timer.mock.hpp"
#endif

namespace robotics::system {
Timer::Timer() : impl_(new Timer::Impl()) {}

void Timer::Start() { impl_->Start(); }

void Timer::Stop() { impl_->Stop(); }

void Timer::Reset() { impl_->Reset(); }

std::chrono::microseconds Timer::ElapsedTime() { return impl_->ElapsedTime(); }
}  // namespace robotics::system