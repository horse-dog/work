// https://www.cnblogs.com/pointer-smq/p/12940360.html

#include <algorithm>
#include <chrono>
#include <deque>
#include <coroutine>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

template <class T>
class Future;

template <class T>
class Promise;

class Schedular;

class SharedStateBase : public std::enable_shared_from_this<SharedStateBase> {
  friend class Schedular;

 public:
  virtual ~SharedStateBase() = default;

 private:
  virtual void invoke_all_callback() = 0;
};

template <class T>
class SharedState : public SharedStateBase {
  friend class Future<T>;
  friend class Promise<T>;

 public:
  SharedState(Schedular& schedular) : schedular(&schedular) {}
  SharedState(const SharedState&) = delete;
  SharedState(SharedState&&) = delete;
  SharedState& operator=(const SharedState&) = delete;
  SharedState& operator=(SharedState&&) = delete;

 private:
  template <class U>
  void set(U&& v) {
    if (settled) {
      return;
    }
    settled = true;
    value = std::forward<U>(v);
    post_all_callbacks();
  }

  T& get() { return value; }

  void add_finish_callback(std::function<void(T&)> callback) {
    finish_callbacks.push_back(std::move(callback));
    if (settled) {
      post_all_callbacks();
    }
  }

  void post_all_callbacks();

  virtual void invoke_all_callback() override {
    callback_posted = false;
    size_t sz = finish_callbacks.size();
    for (size_t i = 0; i != sz; i++) {
      auto v = std::move(finish_callbacks[i]);
      v(value);
    }
    finish_callbacks.erase(finish_callbacks.begin(),
                           finish_callbacks.begin() + sz);
  }

  bool settled = false;
  bool callback_posted = false;
  Schedular* schedular = nullptr;
  T value;
  std::vector<std::function<void(T&)>> finish_callbacks;
};

template <class T>
class Promise {
 public:
  Promise(Schedular& schedular)
      : _schedular(&schedular),
        _state(std::make_shared<SharedState<T>>(*_schedular)) {}

  Future<T> get_future();

  // 协程接口
  Future<T> get_return_object();
  std::suspend_never initial_suspend() { return {}; }
  std::suspend_never final_suspend() noexcept { return {}; }
  void return_value(T v) { set_result(v); }
  void unhandled_exception() { std::terminate(); }
  // 协程接口

  template <class U>
  void set_result(U&& value) {
    if (_state->settled) {
      throw std::invalid_argument("already set result");
    }
    _state->set(std::forward<U>(value));
  }

 private:
  Schedular* _schedular;
  std::shared_ptr<SharedState<T>> _state;
};

template <class T>
class Future {
 public:
  using result_type = T;
  using promise_type = Promise<T>;
  friend class Promise<T>;

 private:
  Future(std::shared_ptr<SharedState<T>> state) : _state(std::move(state)) {}

 public:
  // 协程接口
  bool await_ready() { return _state->settled; }
  void await_suspend(std::coroutine_handle<> handle) {
    add_finish_callback([=](T&) mutable { handle.resume(); });
  }
  T await_resume() { return _state->value; }
  // 协程接口

  void add_finish_callback(std::function<void(T&)> callback) {
    _state->add_finish_callback(std::move(callback));
  }

 private:
  std::shared_ptr<SharedState<T>> _state;
};

template <class T>
Future<T> Promise<T>::get_future() {
  return Future<T>(_state);
}

template <class T>
Future<T> Promise<T>::get_return_object() {
  return get_future();
}

namespace chrono = std::chrono;

class Schedular {
  template <class T>
  friend class SharedState;

 public:
  using timer_callback = std::function<void()>;
  using timer_item = std::tuple<
      bool, float,
      chrono::time_point<chrono::steady_clock, chrono::duration<float>>,
      timer_callback>;
  using timer = std::chrono::steady_clock;
  struct timer_item_cmp {
    bool operator()(const timer_item& a, const timer_item& b) const {
      return std::get<2>(a) > std::get<2>(b);
    }
  };

  Schedular() = default;
  Schedular(Schedular&&) = delete;
  Schedular(const Schedular&) = delete;
  Schedular& operator=(Schedular&&) = delete;
  Schedular& operator=(const Schedular&) = delete;

  void poll() {
    size_t sz = pending_states.size();
    for (size_t i = 0; i != sz; i++) {
      auto state = std::move(pending_states[i]);
      state->invoke_all_callback();
    }
    pending_states.erase(pending_states.begin(), pending_states.begin() + sz);
    if (timer_queue.empty()) {
      return;
    }
    if (pending_states
            .empty()) {  // 如果pending_states为空，则可以sleep较长的时间，等待第一个将要完成的timer
      std::this_thread::sleep_until(std::get<2>(timer_queue.front()));
      auto now = timer::now();
      do {
        deal_one_timer();
      } while (!timer_queue.empty() && std::get<2>(timer_queue.front()) <= now);
    } else {  // 否则只能处理当帧到期的timer，不能sleep，要及时返回给caller，让caller及时下一次poll处理剩下的pending_states
      auto now = timer::now();
      while (!timer_queue.empty() && std::get<2>(timer_queue.front()) <= now) {
        deal_one_timer();
      }
    }
  }

  template <class F, class... Args>
  auto run_until_complete(F&& fn, Args&&... args) ->
      typename std::invoke_result_t<F&&, Args&&...>::result_type {
    auto future = std::forward<F>(fn)(std::forward<Args>(args)...);
    while (!future.await_ready()) {
      poll();
    }
    return future.await_resume();
  }

  void add_timer(bool repeat, float delay, timer_callback callback) {
    auto cur_time =
        chrono::time_point_cast<chrono::duration<float>>(timer::now());
    auto timeout = cur_time + chrono::duration<float>(delay);
    timer_queue.emplace_back(repeat, delay, timeout, callback);
    std::push_heap(timer_queue.begin(), timer_queue.end(), timer_item_cmp{});
  }

  Future<float> delay(float second) {
    auto promise = Promise<float>(*this);
    add_timer(false, second, [=]() mutable { promise.set_result(second); });
    return promise.get_future();
  }

 private:
  void deal_one_timer() {
    std::pop_heap(timer_queue.begin(), timer_queue.end(), timer_item_cmp{});
    auto item = std::move(timer_queue.back());
    timer_queue.pop_back();
    std::get<3>(item)();
    if (std::get<0>(item)) {
      add_timer(true, std::get<1>(item), std::move(std::get<3>(item)));
    }
  }

  void post_call_state(std::shared_ptr<SharedStateBase> state) {
    pending_states.push_back(std::move(state));
  }
  std::vector<std::shared_ptr<SharedStateBase>> pending_states;
  std::deque<timer_item> timer_queue;
};

template <class T>
void SharedState<T>::post_all_callbacks() {
  if (callback_posted) {
    return;
  }
  callback_posted = true;
  schedular->post_call_state(shared_from_this());
}

Future<float> func(Schedular& schedular) {
  std::cout << "start sleep\n";
  auto r = co_await schedular.delay(1.2);
  co_return r;
}

Future<int> func2(Schedular& schedular) {
  auto r = co_await func(schedular);
  std::cout << "slept for " << r << "s\n";
  co_return 42;
}

int main() {
  Schedular schedular;

  auto r = schedular.run_until_complete(func2, schedular);

  std::cout << "run complete with " << r << "\n";
}