// utils.cpp: 定义应用程序的入口点。
//

#include "utils.h"
#include "queue.hpp"
#include "recognition.h"
using namespace std;

threadsafe_queue<int> test_queue;

static void product() {
  while (true) {
    test_queue.push(10);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    test_queue.push(1000);
    return;
  }
}

static void consumer() {
  while (true) {
    auto data = test_queue.wait_and_pop();
    std::cout << *data << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }
}

int main() {
  // auto thread_consumer = std::thread(&consumer);
  // auto thread_product = std::thread(&product);
  // thread_product.join();
  // thread_consumer.join();

  std::vector<Recognition *> recognitions;
  for (auto i = 0; i < 1; ++i) {
    auto *recognition = Recognition::create();
    recognitions.push_back(recognition);
  }

  static auto number = 0;
  auto loop_times = 100;
  while (loop_times--) {
    for (auto *recognition : recognitions) {
      recognition->AddData(number++);
    }
  }

  std::this_thread::sleep_for(10s);
  for (auto* recognition : recognitions) {
      recognition->Release();
  }
  return 0;
}
