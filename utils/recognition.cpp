#include "recognition.h"
Recognition *Recognition::create() { return new Recognition(); }

void Recognition::AddData(int number) {
  {
    std::lock_guard locker(mutex_);
    queue_.emplace(number);
  }
  condition_variable_.notify_one();
}

void Recognition::TransferData(std::queue<int> *destination_queue) {
  std::lock_guard locker(mutex_);
  while (!queue_.empty()) {
    destination_queue->push(queue_.front());
    queue_.pop();
  }
}
void Recognition::Release() {
  running_ = false;
  if (thread_.joinable())
    thread_.join();
  while (!queue_.empty()) {
    queue_.pop();
  }

  worker_->Erase();
  worker_ = nullptr;
}

Recognition::Recognition() {
  worker_ = std::make_shared<Worker>();
  worker_->Attach();
  running_ = true;
  thread_ = std::thread(&Recognition::deal_data, this);
}
Recognition::~Recognition() {}

void Recognition::deal_data() {
  auto number = 0;
  while (running_) {
    {
      std::unique_lock<std::mutex> locker(mutex_);
      condition_variable_.wait(locker,
                               [&]() { return !queue_.empty() || !running_; });
      if (!running_)
        break;
      number = queue_.front();
      queue_.pop();
    }
    number += 1;
    worker_->AddData(number);
  }
}