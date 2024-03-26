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
#include <unistd.h>
#include <sys/syscall.h>
pid_t GetThreadID() {
  pid_t tid;
  tid = syscall(SYS_gettid);
  return tid;
}


class thread_pool {

public:
  thread_pool(int min_workers, int max_workers, int linger) 
  : m_tasks(std::make_shared<TaskQ>(min_workers, max_workers, linger))
  { }

  thread_pool(const thread_pool&) = delete;

  thread_pool& operator=(const thread_pool&) = delete;

  ~thread_pool() {
    if (m_tasks) {
      {
        std::lock_guard<std::mutex> lock(m_tasks->mtx);
        m_tasks->exit = true;
      }
      m_tasks->cv.notify_all();
    }
    for (auto&& thread : m_threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  template <class _Fn>
  void addTask(_Fn&& __f) {
    m_tasks->mtx.lock();
    m_tasks->tasks.emplace_back(std::forward<_Fn>(__f));
    if (m_tasks->num_idle > 0) {
      m_tasks->mtx.unlock();
      m_tasks->cv.notify_one();
      return;
    }
    
    if (m_threads.size() < m_tasks->max_workers) {
      __createWorker();
    }
    m_tasks->mtx.unlock();
  }

protected:
  void __createWorker() {
    m_threads.emplace_back([taskQ = m_tasks, this] {
      printf("%d: start...\n", GetThreadID());
      std::unique_lock<std::mutex> lock(taskQ->mtx);
      ++taskQ->num_idle;
      while (true) {
        if (!taskQ->tasks.empty()) {
          // get and exec.
          auto task = taskQ->tasks.front();
          taskQ->tasks.pop_front();
          --taskQ->num_idle;
          lock.unlock();
          task(); // execute the task.
          lock.lock();
          ++taskQ->num_idle;
        } else if (taskQ->exit) {
          --taskQ->num_idle;
          break;
        } else {
          // wait for tasks.
          auto r = taskQ->cv.wait_for(lock, std::chrono::seconds(taskQ->linger));
          if (r == std::cv_status::timeout 
            && this->m_threads.size() > taskQ->min_workers) {
            --taskQ->num_idle;

            auto it = std::find_if(m_threads.begin(), m_threads.end(), 
              [&](const std::thread& t) {
                return t.get_id() == std::this_thread::get_id();
            });
            if (it != m_threads.end()) {
              it->detach();
              m_threads.erase(it);
            }
            printf("%d: exit...\n", GetThreadID());
            break;
          }
        }
      }
      
    });
  }

private:
  using task = std::function<void()>;
  struct TaskQ {
    bool exit;  // thread pool exit or not.
    const int min_workers;
    const int max_workers;
    const int linger; // seconds.
    int num_idle; // num of idle threads.
    std::list<task> tasks;
    std::condition_variable cv;
    std::mutex mtx;

    TaskQ(int min, int max, int linger) 
    : exit(false), min_workers(min), max_workers(max), 
      linger(linger), num_idle(0) 
    { }
  };

  std::list<std::thread> m_threads;
  std::shared_ptr<TaskQ> m_tasks;

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
class CouldNotDerive1 : virtual public A<CouldNotDerive1> {};

class CouldNotDerive2 final {};


// LRU
class LRUCache {
protected:
    struct DLinkedNode {
        int _M_key;
        int _M_val;
        DLinkedNode* _M_prev = 0;
        DLinkedNode* _M_next = 0;
    };

    struct DLinkedList {
        DLinkedNode _M_head;
        DLinkedNode _M_tail;
        
        DLinkedList() {
            _M_head._M_next = &_M_tail;
            _M_tail._M_prev = &_M_head;
        }

        ~DLinkedList() {
            DLinkedNode* __node = _M_head._M_next;
            while (__node != &_M_tail) {
                DLinkedNode* __next = __node->_M_next;
                delete __node;
                __node = __next;
            }
        }

        DLinkedNode* Front() const {
            return _M_head._M_next == &_M_tail ? 0 : _M_head._M_next;
        }

        DLinkedNode* Back() const {
            return _M_tail._M_prev == &_M_head ? 0 : _M_tail._M_prev;
        }

        void MoveToHead(DLinkedNode* __node) {
            _M_remove(__node);
            _M_pushFront(__node);
        }

        void MoveToTail(DLinkedNode* __node) {
            _M_remove(__node);
            _M_pushBack(__node);
        }

        void PushFront(int __k, int __v) {
            DLinkedNode* __node = new DLinkedNode{};
            __node->_M_key = __k;
            __node->_M_val = __v;
            _M_pushFront(__node);
        }

        void PushBack(int __k, int __v) {
            DLinkedNode* __node = new DLinkedNode{};
            __node->_M_key = __k;
            __node->_M_val = __v;
            _M_pushBack(__node);
        }

        void PopFront() {
            if (!Empty()) {
                DLinkedNode* __head = _M_head._M_next;
                _M_remove(__head);
                delete __head;
            }
        }

        void PopBack() {
            if (!Empty()) {
                DLinkedNode* __tail = _M_tail._M_prev;
                _M_remove(__tail);
                delete __tail;
            }
        }

        void Remove(DLinkedNode* __node) {
            _M_remove(__node);
            delete __node;
        }

        void MoveToAnotherHead(DLinkedNode* __node, DLinkedList& __list) {
            _M_remove(__node);
            __list._M_pushFront(__node);
        }

        void MoveToAnotherTail(DLinkedNode* __node, DLinkedList& __list) {
            _M_remove(__node);
            __list._M_pushBack(__node);
        }

        bool Empty() const {
            return _M_head._M_next == &_M_tail;
        }

    protected:
        void _M_remove(DLinkedNode* __node) {
            if (__node == 0) return;
            // __prev <-> __node <-> __next.
            DLinkedNode* __prev = __node->_M_prev;
            DLinkedNode* __next = __node->_M_next;
            __prev->_M_next = __next;
            __next->_M_prev = __prev;
        }

        void _M_pushFront(DLinkedNode* __node) {
            if (__node == 0) return;
            __node->_M_prev = &_M_head;
            __node->_M_next = _M_head._M_next;
            _M_head._M_next->_M_prev = __node;
            _M_head._M_next = __node;
        }
        
        void _M_pushBack(DLinkedNode* __node) {
            if (__node == 0) return;
            __node->_M_next = &_M_tail;
            __node->_M_prev = _M_tail._M_prev;
            _M_tail._M_prev->_M_next = __node;
            _M_tail._M_prev = __node;
        }
    };

public:
    LRUCache(int capacity) { _M_cap = capacity; }

    int get(int key) {
        auto __it = _M_cache.find(key);
        if (__it == _M_cache.end()) {
            return -1;
        }
        // key is exist.
        DLinkedNode* __node = __it->second;
        _M_lru_list.MoveToHead(__node);
        return __node->_M_val;
    }

    void put(int key, int value) {
        auto __it = _M_cache.find(key);
        if (__it == _M_cache.end()) {
            if (_M_cache.size() == _M_cap) {
                DLinkedNode* __last = _M_lru_list.Back();
                _M_cache.erase(__last->_M_key);
                _M_lru_list.PopBack();
            }
            _M_lru_list.PushFront(key, value);
            _M_cache.emplace(key, _M_lru_list.Front());
        } else {
            // key is exist, refresh it.
            DLinkedNode* __node = __it->second;
            __node->_M_val = value;
            _M_lru_list.MoveToHead(__node);
        }
    }

private:
    int _M_cap; // always > 0.
    std::unordered_map<int, DLinkedNode*> _M_cache;
    DLinkedList _M_lru_list;
};

// LFU
class LFUCache {
protected:
    struct DLinkedNode {
        int _M_key;
        int _M_val;
        int _M_use_count = 0;
        DLinkedNode* _M_prev = 0;
        DLinkedNode* _M_next = 0;
    };

    struct DLinkedList {
        DLinkedNode _M_head;
        DLinkedNode _M_tail;
        
        DLinkedList() {
            _M_head._M_next = &_M_tail;
            _M_tail._M_prev = &_M_head;
        }

        ~DLinkedList() {
            DLinkedNode* __node = _M_head._M_next;
            while (__node != &_M_tail) {
                DLinkedNode* __next = __node->_M_next;
                delete __node;
                __node = __next;
            }
        }

        DLinkedNode* Front() const {
            return _M_head._M_next == &_M_tail ? 0 : _M_head._M_next;
        }

        DLinkedNode* Back() const {
            return _M_tail._M_prev == &_M_head ? 0 : _M_tail._M_prev;
        }

        void MoveToHead(DLinkedNode* __node) {
            _M_remove(__node);
            _M_pushFront(__node);
        }

        void MoveToTail(DLinkedNode* __node) {
            _M_remove(__node);
            _M_pushBack(__node);
        }

        void PushFront(int __k, int __v) {
            DLinkedNode* __node = new DLinkedNode{};
            __node->_M_key = __k;
            __node->_M_val = __v;
            __node->_M_use_count = 1;
            _M_pushFront(__node);
        }

        void PushBack(int __k, int __v) {
            DLinkedNode* __node = new DLinkedNode{};
            __node->_M_key = __k;
            __node->_M_val = __v;
            __node->_M_use_count = 1;
            _M_pushBack(__node);
        }

        void PopFront() {
            if (!Empty()) {
                DLinkedNode* __head = _M_head._M_next;
                _M_remove(__head);
                delete __head;
            }
        }

        void PopBack() {
            if (!Empty()) {
                DLinkedNode* __tail = _M_tail._M_prev;
                _M_remove(__tail);
                delete __tail;
            }
        }

        void Remove(DLinkedNode* __node) {
            _M_remove(__node);
            delete __node;
        }

        void MoveToAnotherHead(DLinkedNode* __node, DLinkedList& __list) {
            _M_remove(__node);
            __list._M_pushFront(__node);
        }

        void MoveToAnotherTail(DLinkedNode* __node, DLinkedList& __list) {
            _M_remove(__node);
            __list._M_pushBack(__node);
        }

        bool Empty() const {
            return _M_head._M_next == &_M_tail;
        }

    protected:
        void _M_remove(DLinkedNode* __node) {
            if (__node == 0) return;
            // __prev <-> __node <-> __next.
            DLinkedNode* __prev = __node->_M_prev;
            DLinkedNode* __next = __node->_M_next;
            __prev->_M_next = __next;
            __next->_M_prev = __prev;
        }

        void _M_pushFront(DLinkedNode* __node) {
            if (__node == 0) return;
            __node->_M_prev = &_M_head;
            __node->_M_next = _M_head._M_next;
            _M_head._M_next->_M_prev = __node;
            _M_head._M_next = __node;
        }
        
        void _M_pushBack(DLinkedNode* __node) {
            if (__node == 0) return;
            __node->_M_next = &_M_tail;
            __node->_M_prev = _M_tail._M_prev;
            _M_tail._M_prev->_M_next = __node;
            _M_tail._M_prev = __node;
        }
    };

public:
    LFUCache(int capacity) {
        _M_capacity = capacity;
    }
    
    int get(int key) {
        auto __it = _M_cache.find(key);
        if (__it == _M_cache.end()) {
            return -1;
        }
        DLinkedNode* __node = __it->second;
        int __freq = __node->_M_use_count++;
        int __value = __node->_M_val;
        auto&& __list1 = _M_freq_cache[__freq];
        auto&& __list2 = _M_freq_cache[__freq + 1];
        __list1.MoveToAnotherHead(__node, __list2);
        if (_M_cur_min_freq == __freq && __list1.Empty()) {
            _M_cur_min_freq += 1;
        }
        return __value;
    }
    
    void put(int key, int value) {
        auto __it = _M_cache.find(key);
        if (__it == _M_cache.end()) {
            if (_M_cache.size() == _M_capacity) {
                auto&& __list = _M_freq_cache[_M_cur_min_freq];
                int __key = __list.Back()->_M_key;
                _M_cache.erase(__key);
                __list.PopBack();
            }
            auto&& __list = _M_freq_cache[1];
            __list.PushFront(key, value);
            _M_cache.emplace(key, __list.Front());
            _M_cur_min_freq = 1;
        } else {
            DLinkedNode* __node = __it->second;
            int __freq = __node->_M_use_count++;
            __node->_M_val = value;
            auto&& __list1 = _M_freq_cache[__freq];
            auto&& __list2 = _M_freq_cache[__freq + 1];
            __list1.MoveToAnotherHead(__node, __list2);
            if (_M_cur_min_freq == __freq && __list1.Empty()) {
                _M_cur_min_freq += 1;
            }
        }
    }

protected:
    std::unordered_map<int, DLinkedNode*> _M_cache;
    std::unordered_map<int, DLinkedList> _M_freq_cache;
    int _M_cur_min_freq = 1;
    int _M_capacity; // always > 0.
};
