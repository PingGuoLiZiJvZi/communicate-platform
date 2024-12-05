#pragma once
#include <cstdlib>
#include <ctime>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
class Thread_Pool {
 public:
 bool isstop = false;
 Thread_Pool(const Thread_Pool&thread_pool)=delete;
 Thread_Pool operator=(Thread_Pool const& thread_pool) =delete;
 Thread_Pool(Thread_Pool const&&thread_pool)           = delete;
 Thread_Pool operator=(Thread_Pool const&&thread_pool) = delete;
 std::mutex mtx;
 std::condition_variable cva;
 std::vector<std::thread> threads;
 std::queue<std::function<void(void)>> tasks;
 explicit Thread_Pool(int thnum) {
   for (int i = 0; i < thnum; i++) {
     threads.emplace_back([this] {
       while (true) {
         std::unique_lock<std::mutex> mtxu(mtx);
         if (isstop && tasks.empty()) {
           return;
         };
         cva.wait(mtxu, [this] { return (!tasks.empty()); });
         auto temp = std::move(tasks.front());
         tasks.pop();
         mtxu.unlock();
         temp();
       }
     });
   }
  }
  template <class T, typename... Args>
  void add_task(T&& t, Args&&... args) {
    std::function<void()> temptask = bind(t, args...);
    std::unique_lock<std::mutex> mtxu(mtx);
    tasks.push(temptask);
    cva.notify_one();
  }
  template <class T>
  void add_task(T&& t) {
    std::function<void()> temptask = t;
    std::unique_lock<std::mutex> mtxu(mtx);
    tasks.push(temptask);
    cva.notify_one();
  }
  ~Thread_Pool() {
    {
      std::unique_lock<std::mutex> mtxu(mtx);
      isstop = true;
    }
    cva.notify_all();
    for (auto&& every_thread : threads) {
      every_thread.join();
    }
  }
};