#include <chrono>
#include <thread>

#include "lock-free-queue/lockfreequeue.h"

struct MyStruct {
  int data[3];
};

auto consumeFunction(LockFreeQueue<MyStruct>* data_queue) {
  using namespace std::literals::chrono_literals;
  std::this_thread::sleep_for(5s);

  while(data_queue->size()) {
    const auto d = data_queue->getNextRead();
    data_queue->updateReadIndex();

    std::cout << "consumeFunction read elem:" 
              << d->data[0] << "," 
              << d->data[1] << "," 
              << d->data[2] << " size:" << data_queue->size() << std::endl;

    std::this_thread::sleep_for(1s);
  }

  std::cout << "consumeFunction exiting." << std::endl;
}

int main(int, char **) {
  // the queue is smaller than the main loop. Therefore it should loop
  LockFreeQueue<MyStruct> data_queue(20);

  std::thread ct(consumeFunction, &data_queue);

  for(auto i = 0; i < 50; ++i) {
    const MyStruct d{i, i * 10, i * 100};
    *data_queue.getNextWrite() = d;
    data_queue.updateWriteIndex();

    std::cout << "main constructed elem:" 
              << d.data[0] << ","
              << d.data[1] << ","
              << d.data[2] << " size:" << data_queue.size() << std::endl;

    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(1s);
  }

  ct.join();

  std::cout << "main exiting." << std::endl;

  return 0;
}
