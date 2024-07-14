#include "worker.h"
#include "detection.h"

void Worker::Attach() { Detection::GetInstance().Attach(shared_from_this()); }

void Worker::Erase() { Detection::GetInstance().Erase(shared_from_this()); }

void Worker::AddData(int num) {
  num += 2;
  std::lock_guard<std::mutex> locker(pretreatment_mutex_);
  pretreatment_vector_.push_back(num);
}

void Worker::StealData(std::vector<int> *wait_for_detection, int *size) {
  std::lock_guard<std::mutex> locker(pretreatment_mutex_);
  if (pretreatment_vector_.empty())
    return;
  wait_for_detection->insert(wait_for_detection->begin(),
                             pretreatment_vector_.begin(),
                             pretreatment_vector_.end());
  *size = static_cast<int>(pretreatment_vector_.size());
  pretreatment_vector_.clear();
}

void Worker::RestoreData(const std::vector<int> &data) {
  if (data.empty())
    return;
  {
    std::lock_guard<std::mutex> locker(postprocessing_mutex_);
    for (const auto &number : data) {
      postprocessing_queue_.push_back(number);
    }
  }
  postprocessing_condition_.notify_one();
}

void Worker::postprocessing_deal() {
  std::deque<int> wait_deal;
  while (postprocessing_running_) {
    {
      std::unique_lock<std::mutex> locker(postprocessing_mutex_);
      postprocessing_condition_.wait(locker, [this]() {
        return !postprocessing_queue_.empty() || !postprocessing_running_;
      });

      if (!postprocessing_running_)
        break;
      wait_deal.swap(postprocessing_queue_);
    }
    // TODO:deal

    wait_deal.clear();
  }
}
Worker::Worker() {
  postprocessing_running_ = true;
  postprocessing_thread_ = std::thread(&Worker::postprocessing_deal, this);
}

Worker::~Worker() {
  postprocessing_running_ = false;
  postprocessing_condition_.notify_one();
  if (postprocessing_thread_.joinable())
    postprocessing_thread_.join();
}