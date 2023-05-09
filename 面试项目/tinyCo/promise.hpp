#pragma once

#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <optional>

#include "result.hpp"
#include "taskAwaiter.hpp"

template <typename ResultType>
class Task;

template <typename ResultType>
struct TaskPromise {
  std::suspend_never initial_suspend() { return {}; }

  std::suspend_always final_suspend() noexcept { return {}; }

  Task<ResultType> get_return_object() {
    return Task{std::coroutine_handle<TaskPromise>::from_promise(*this)};
  }

  template <typename _ResultType>
  TaskAwaiter<_ResultType> await_transform(Task<_ResultType> &&task) {
    return TaskAwaiter<_ResultType>(std::move(task));
  }

  void unhandled_exception() {
    std::lock_guard lock(completion_lock);
    result = Result<ResultType>(std::current_exception());
    completion.notify_all();
    notify_callbacks();
  }

  void return_value(ResultType value) {
    std::lock_guard lock(completion_lock);
    result = Result<ResultType>(std::move(value));
    completion.notify_all();
    notify_callbacks();
  }

  ResultType get_result() {
    // blocking for result or throw on exception
    std::unique_lock lock(completion_lock);
    if (!result.has_value()) {
      completion.wait(lock);
    }
    return result->get_or_throw();
  }

  void on_completed(std::function<void(Result<ResultType>)> &&func) {
    std::unique_lock lock(completion_lock);
    if (result.has_value()) {
      auto value = result.value();
      lock.unlock();
      func(value);
    } else {
      completion_callbacks.push_back(func);
    }
  }

 private:
  std::optional<Result<ResultType>> result;

  std::mutex completion_lock;
  std::condition_variable completion;

  std::list<std::function<void(Result<ResultType>)>> completion_callbacks;

  void notify_callbacks() {
    auto value = result.value();
    for (auto &callback : completion_callbacks) {
      callback(value);
    }
    completion_callbacks.clear();
  }
};
