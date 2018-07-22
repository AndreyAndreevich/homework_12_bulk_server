#ifndef event_loop_h
#define event_loop_h

#include "Storage.h"
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>

enum class EventCode { 
  NOW,
  ASYNC
};

class EventLoop {
  bool quit{false};
  std::queue<std::function<void()>> event_queue;
  std::thread work_thread;
  std::mutex mtx;
  std::condition_variable c_v;

  std::queue<std::string> error_queue;
  std::mutex err_mtx;

private:
  EventLoop(Storage&) = delete;
  EventLoop& operator=(EventLoop&) = delete;
  EventLoop();
  ~EventLoop();

public:
  static EventLoop& Instance(); 
  void send(EventCode, const std::function<void()>&);
  std::string getException();
  size_t count();
};

#endif