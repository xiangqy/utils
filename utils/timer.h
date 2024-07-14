#ifndef TIMER_H_
#define TIMER_H_

#include <functional>
#include <thread>

class Timer final{
public:
  using TimerCallback = std::function<void()>;

  explicit Timer(int interval, TimerCallback callback);

  ~Timer();

  void Start();

  void Stop();

private:
  class Imple {
  public:
    Imple(int interval, TimerCallback function)
        : interval_(interval), function_( std::move(function)) {}
    int interval_;
    TimerCallback function_;
    std::atomic_bool active_{true};
  };

  void thread_function();
  std::shared_ptr<Imple> imple_ptr_;
  std::thread thread_;
  std::chrono::time_point<std::chrono::steady_clock> clock_;
};

#endif