#include "EventLoop.h"
#include "Handler.h"

#include <chrono>

EventLoop::EventLoop() {
  work_thread = std::thread([this](){
  while (!quit) {
    if (!event_queue.empty()) {
      std::unique_lock<std::mutex> lk(mtx);
      auto ev = event_queue.front();
      event_queue.pop();
      try {
        ev();
      } catch(const std::exception& e) {
        std::unique_lock<std::mutex> err_lk(err_mtx);
        error_queue.push(e.what());
      }
    } else {
      std::unique_lock<std::mutex> lk(mtx);
      c_v.wait(lk,[this](){return quit || !event_queue.empty();});
    }
  }
  });
}

EventLoop::~EventLoop() {
  quit = true;
  c_v.notify_one();
  work_thread.join();
}

EventLoop& EventLoop::Instance() {
  static EventLoop instance;
  return instance;
}

void EventLoop::send(EventCode code, const std::function<void()>& func) {
  std::unique_lock<std::mutex> lk(mtx);
  switch(code) {
  case EventCode::NOW: 
    func();
    break;
  case EventCode::ASYNC:
    event_queue.push(func);
    c_v.notify_one();
    break;
  }
}

std::string EventLoop::getException() 
{
  std::string e;
  std::unique_lock<std::mutex> lk(err_mtx);
  if (error_queue.size()) {
    e = error_queue.front();
    error_queue.pop();
  }
  return e;
}

size_t EventLoop::count()
{
  std::unique_lock<std::mutex> lk(mtx);
  return event_queue.size();
}