#include "timer.h"

Timer::Timer(int interval, TimerCallback callback) {
  imple_ptr_ = std::make_shared<Imple>(interval, std::move(callback));
}
Timer::~Timer() = default;

void Timer::Start() {
  if (thread_.joinable())
    return;

  clock_ = std::chrono::steady_clock::now();
  thread_ = std::thread(&Timer::thread_function, this);
}

void Timer::Stop() {
  imple_ptr_->active_.store(false);
  if (thread_.joinable())
    thread_.join();
}

void Timer::thread_function() {
  if (!imple_ptr_->active_.load())
    return;
  imple_ptr_->function_();
  auto current_clock = std::chrono::steady_clock::now();
  auto real_interval = std::chrono::duration_cast<std::chrono::milliseconds>(
                           current_clock - clock_)
                           .count() -
                       imple_ptr_->interval_;
  if (real_interval < 0) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(std::abs(real_interval)));
  }
  clock_ = current_clock;
}