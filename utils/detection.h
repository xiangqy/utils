#ifndef DETECTION_H_
#define DETECTION_H_

#include <list>
#include <mutex>
#include <map>
#include <tuple>
#include "timer.h"

class Worker;
class Detection {
public:
  static Detection &GetInstance();

  void Attach(std::shared_ptr<Worker> worker);

  void Erase(std::shared_ptr<Worker> worker);

private:
  Detection();
  ~Detection();

  void deal_detections();
  void distribute_detections(const std::vector<std::vector<int>>& inference_result);

  bool inference(const std::vector<int> &pre_vector,
                 std::vector<std::vector<int>> *inference_result);

  std::mutex mutex_;
  std::list<std::shared_ptr<Worker>> list_;

  std::map<std::shared_ptr<Worker>, std::tuple<int, int>> marks_;

  std::vector<int> wait_for_deal_;

  Timer timer_;
};

#endif
