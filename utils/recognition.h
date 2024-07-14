#ifndef RECOGNITION_H_
#define RECOGNITION_H_

#include "worker.h"
#include <queue>

class Recognition {
public:
  static Recognition *create();
  void AddData(int number);

  void TransferData(std::queue<int> *destination_queue);

  void Release();

private:
  Recognition();
  ~Recognition();

  void deal_data();

  std::mutex mutex_;
  std::queue<int> queue_;
  std::thread thread_;
  bool running_{false};
  std::condition_variable condition_variable_;

  std::shared_ptr<Worker> worker_;
};

#endif