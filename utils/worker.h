#ifndef WORKER_H_
#define WORKER_H_
#include "detection.h"
#include <deque>
#include <memory>
#include <vector>

class Worker : public std::enable_shared_from_this<Worker> {
public:
  Worker();
  ~Worker();

  void Attach();

  void Erase();

  void AddData(int num);

  void StealData(std::vector<int> *wait_for_detection, int *size);

  void RestoreData(const std::vector<int> &data);

private:
  void postprocessing_deal();

  std::vector<int> pretreatment_vector_;
  std::mutex pretreatment_mutex_;

  std::deque<int> postprocessing_queue_;
  std::mutex postprocessing_mutex_;
  std::condition_variable postprocessing_condition_;
  std::thread postprocessing_thread_;
  bool postprocessing_running_{false};
};

#endif