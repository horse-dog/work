#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <thread>
#ifdef __linux__
#include <semaphore.h>
#endif

// 无锁链栈
template <typename T>
class LockFreeStack {
 public:
  LockFreeStack() : list_head(nullptr) {}

  ~LockFreeStack() {
    Node* p = list_head;
    while (p != nullptr) {
      Node* tmp = p->next;
      delete p;
      p = tmp;
    }
    list_head = nullptr;
  }

  bool push(const T& value) {
    Node* p = new Node{value, list_head};
    while (list_head.compare_exchange_weak(p->next, p) == false)
      ;
    return true;
  }

  bool pop(T& value) {
    Node* p = list_head;
    while (p != nullptr && list_head.compare_exchange_weak(p, p->next) == false)
      ;
    if (p == nullptr) return false;
    value = std::move(p->value);
    delete p;
    return true;
  }

  // just for debug
  void disp() const {
    for (Node* it = list_head; it != nullptr; it = it->next)
      printf("%d ", it->value);
    printf("\n");
  }

 private:
  struct Node {
    T value;
    Node* next;
  };

  std::atomic<Node*> list_head;
};

// 用户态自旋锁
class SpinLock {
#ifndef __linux__
 public:
  SpinLock() noexcept {};
  void lock() noexcept {
    while (flag.test_and_set(std::memory_order_acquire))
      ;
  }
  void unlock() noexcept { flag.clear(std::memory_order_release); }
  bool try_lock() noexcept {
    return !flag.test_and_set(std::memory_order_acquire);
  }

 private:
  std::atomic_flag flag = ATOMIC_FLAG_INIT;
#else
 public:
  SpinLock() noexcept { pthread_spin_init(&locker, 0); };

  ~SpinLock() noexcept { pthread_spin_destroy(&locker); }

  void lock() noexcept { pthread_spin_lock(&locker); }

  void unlock() noexcept { pthread_spin_unlock(&locker); }

  bool try_lock() noexcept { return pthread_spin_trylock(&locker) == 0; }

 private:
  pthread_spinlock_t locker;
#endif
};

// 单进程用户态信号量
class Semaphore {
#ifndef __linux__
 public:
  Semaphore(int count = 0) : count(count){};

  void signal() {
    {
      std::unique_lock<std::mutex> lock(mut);
      ++count;
    }
    cv.notify_one();
  }

  void wait() {
    std::unique_lock<std::mutex> lock(mut);
    while (count <= 0) {
      cv.wait(lock);
    }
    --count;
  }

 private:
  std::mutex mut;
  std::condition_variable cv;
  int count;
#else
 public:
  Semaphore(int count = 0) { sem_init(&sem, 0, count); }

  ~Semaphore() { sem_destroy(&sem); }

  void post() { sem_post(&sem); }

  void wait() { sem_wait(&sem); }

 private:
  sem_t sem;
#endif
};

// 可伸缩线程池
class ThreadPool {
 public:
  ThreadPool(int min, int max, int linger)
      : tskQptr(std::make_shared<taskQ>(min, max, linger)) {}

  ~ThreadPool() {
    if (static_cast<bool>(tskQptr)) {
      {
        std::lock_guard locker(tskQptr->mtx);
        tskQptr->exit = true;
      }
      tskQptr->cv.notify_all();
    }
    for (auto&& t : threads) t.join();
  }

  template <class F>
  void addTask(F&& task) {
    {
      std::lock_guard locker(tskQptr->mtx);
      tskQptr->tasks.emplace_back(std::forward<F>(task));
    }
    if (tskQptr->idleCnt > 0)
      tskQptr->cv.notify_one();
    else if (threads.size() < tskQptr->maxWorkers) {
      createWorker();
    }
  }

  void createWorker() {
    threads.emplace_back([taskQPtr = tskQptr] {
      std::unique_lock locker(taskQPtr->mtx);
      taskQPtr->idleCnt++;
      while (true) {
        if (!taskQPtr->tasks.empty()) {
          auto task = std::move(taskQPtr->tasks.front());
          taskQPtr->tasks.pop_front();
          taskQPtr->idleCnt--;
          locker.unlock();
          std::invoke(task);
          locker.lock();
          taskQPtr->idleCnt++;
        } else if (taskQPtr->exit) {
          taskQPtr->idleCnt--;
          break;
        } else {
          auto r = taskQPtr->cv.wait_for(
              locker, std::chrono::seconds(taskQPtr->linger));
          if (r == std::cv_status::timeout &&
              taskQPtr->tasks.size() > taskQPtr->minWorkers) {
            taskQPtr->idleCnt--;
            break;
          }
        }
      }
    });
  }

 private:
  using task = std::function<void(void)>;
  struct taskQ {
    bool exit;
    const int minWorkers;
    const int maxWorkers;
    const int linger;
    int idleCnt;
    std::list<task> tasks;
    SpinLock mtx;
    std::condition_variable_any cv;

    taskQ(int min, int max, int lin)
        : exit(false),
          minWorkers(min),
          maxWorkers(max),
          linger(lin),
          idleCnt(0) {}
  };
  std::shared_ptr<taskQ> tskQptr;
  std::list<std::thread> threads;
};

// 不能被继承的类
template <class T>
class A {
  friend T;

 private:
  A(){};
  ~A(){};
};

// virtual关键字很重要
class CouldNotDerive : virtual public A<CouldNotDerive> {};

class C : CouldNotDerive {};