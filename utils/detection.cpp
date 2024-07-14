#include "detection.h"

#include "worker.h"

Detection &Detection::GetInstance() {
  static Detection instance;
  return instance;
}

void Detection::Attach(std::shared_ptr<Worker> worker) {
  std::lock_guard locker(mutex_);
  list_.push_back(std::move(worker));
}

void Detection::Erase(std::shared_ptr<Worker> worker) {
  std::lock_guard locker(mutex_);
  std::erase_if(list_, [=](const std::shared_ptr<Worker> &iter) {
    return iter == worker;
  });
  std::erase_if(
      marks_, [=](const std::pair<std::shared_ptr<Worker>, std::tuple<int, int>>
                      &pair) { return pair.first == worker; });
}

Detection::Detection() : timer_(500, [this] { deal_detections(); }) {
  timer_.Start();
}

Detection::~Detection() { timer_.Stop(); }

void Detection::deal_detections() {
  auto size = 0;
  wait_for_deal_.clear();
  marks_.clear();
  {
    std::lock_guard<std::mutex> locker(mutex_);
    std::erase_if(list_, [](const std::shared_ptr<Worker> &worker) {
      return worker == nullptr;
    });
    for (const auto &iter : list_) {
      if (iter == nullptr) {
        continue;
      }
      auto start = static_cast<int>(wait_for_deal_.size());
      iter->StealData(&wait_for_deal_, &size);
      marks_.insert(std::make_pair(iter, std::make_tuple(start, size)));
    }
  }
  // TODO:deal
  std::vector<std::vector<int>> inference_result;
  inference(wait_for_deal_, &inference_result);

  distribute_detections(inference_result);
}

void Detection::distribute_detections(
    const std::vector<std::vector<int>> &inference_result) {
  std::lock_guard<std::mutex> locker(mutex_);
  for (const auto &pair : marks_) {
    if (pair.first == nullptr)
      continue;
    auto [start, size] = pair.second;
    for (const auto &iter : inference_result)
      pair.first->RestoreData(iter);
  }
}

bool Detection::inference(const std::vector<int> &pre_vector,
                          std::vector<std::vector<int>> *inference_result) {
  if (pre_vector.empty())
    return false;
  for (auto iter : pre_vector) {
    inference_result->push_back(std::vector<int>{iter + 10, iter + 11});
  }
  return true;
}